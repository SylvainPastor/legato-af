//--------------------------------------------------------------------------------------------------
/**
 * Implementation of the routines for building Executables.
 *
 * Copyright (C) 2013-2014, Sierra Wireless Inc.  Use of this work is subject to license.
 */
//--------------------------------------------------------------------------------------------------

#include "LegatoObjectModel.h"
#include "ExecutableBuilder.h"
#include "Utilities.h"

//--------------------------------------------------------------------------------------------------
/**
 * Generate component initializer declarations and functions calls for a given component
 * and all its sub-functions.
 */
//--------------------------------------------------------------------------------------------------
static void GenerateInitFunctionCalls
(
    legato::Component& component,
    std::set<std::string>& completedSet,    ///< Set of components already initialized.
    std::stringstream& compInitDeclBuffer,  ///< Buffer to put init function declarations in.
    std::stringstream& compInitBuffer       ///< Buffer to put init function calls in.
)
//--------------------------------------------------------------------------------------------------
{
    std::string componentInitFuncName = mk::GetComponentInitName(component);

    // If I haven't already been initialized,
    if (completedSet.find(componentInitFuncName) == completedSet.end())
    {
        // Check if I am my own ancestor.
        if (component.BeingProcessed())
        {
            throw legato::DependencyException("Dependency loop detected: " + component.Name());
        }

        // Flag myself as being processed.
        component.BeingProcessed(true);

        // Go down to the next level first (depth first traversal).
        for (auto& mapEntry : component.SubComponents())
        {
            legato::Component* subComponentPtr = mapEntry.second;

            if (subComponentPtr == NULL)
            {
                throw legato::Exception("Unresolved sub-component '" + mapEntry.first + "'"
                                        " of component '" + component.Name() + "'.");
            }

            try
            {
                GenerateInitFunctionCalls(*subComponentPtr,
                                          completedSet,
                                          compInitDeclBuffer,
                                          compInitBuffer);
            }
            catch (legato::DependencyException e)
            {
                throw legato::DependencyException(std::string(e.what()) + " <- "
                                                  + component.Name());
            }
        }

        // Generate forward declaration of the component initializer.
        compInitDeclBuffer << "void " << componentInitFuncName << "(void);" << std::endl;

        // Generate call to queue the initialization function to the event loop.
        compInitBuffer << "    event_QueueComponentInit(" << componentInitFuncName << ");"
                       << std::endl;

        // Add myself to the list of things that have already been initialized.
        completedSet.insert(componentInitFuncName);

        // Flag myself as finished being processed.
        component.BeingProcessed(false);
    }
}


//--------------------------------------------------------------------------------------------------
/**
 * Generate component initializer declarations and functions calls for all components in an
 * executable.
 */
//--------------------------------------------------------------------------------------------------
static void GenerateInitFunctionCalls
(
    legato::Executable& executable,
    std::stringstream& compInitDeclBuffer,  ///< Buffer to put init function declarations in.
    std::stringstream& compInitBuffer       ///< Buffer to put init function calls in.
)
//--------------------------------------------------------------------------------------------------
{
    // Use a recursive, depth-first tree walk over the tree starting with the list of
    // Component Instances and their Components, and going down through sub-components.
    // So that initialization happens in the correct order (lower-level
    // stuff gets initialized before the higher-level stuff that uses it).

    // Use a set to keep track of which components have already been initialized, so we
    // don't initialize the same component twice.
    std::set<std::string> completedSet;

    for (auto& componentInstance : executable.ComponentInstances())
    {
        GenerateInitFunctionCalls(componentInstance.GetComponent(),
                                  completedSet,
                                  compInitDeclBuffer,
                                  compInitBuffer);
    }

    // If the Default Component has at least one source file (besides the one we are
    // auto-generating right now),
    const legato::Component& defaultComponent = executable.DefaultComponent();
    if (!(defaultComponent.CSourcesList().empty()))
    {
        // Generate forward declaration of the component initializer.
        std::string componentInitFuncName = mk::GetComponentInitName(defaultComponent);
        compInitDeclBuffer << "void " << componentInitFuncName << "(void);" << std::endl;

        // Queue up the component initializer to be called when the Event Loop starts.
        compInitBuffer << "    event_QueueComponentInit(" << componentInitFuncName << ");"
                       << std::endl;
    }
}


//--------------------------------------------------------------------------------------------------
/**
 * Generates a main .c for the executable.
 */
//--------------------------------------------------------------------------------------------------
void ExecutableBuilder_t::GenerateMain
(
    legato::Executable& executable
)
//--------------------------------------------------------------------------------------------------
{
    // Generate the path to the directory in which the generated files will go.
    std::string path = m_Params.ObjOutputDir()
                     + "/"
                     + executable.CName();

    // Make sure the directory exists.
    legato::MakeDir(path);

    // Add "/_main.c" to get the path of the file to generate code into.
    path += "/_main.c";

    if (m_Params.IsVerbose())
    {
        std::cout << "Generating startup code for '" << executable.OutputPath()
                  << "' in '" << path << "'." << std::endl;
    }

    // Open the file as an output stream.
    std::ofstream outputFile(path);

    if (outputFile.is_open() == false)
    {
        throw legato::Exception("Could not open, '" + path + ",' for writing.");
    }

    // The code is broken down into multiple secions.  This is to make writing the code
    // generator a little easier.  We just have to stream the various parts into the
    // required sections, so we only need to loop over our data structures once.
    std::stringstream compInitDeclBuffer,
                      sessionBuffer,
                      filterBuffer,
                      serverInitDeclBuffer,
                      clientInitDeclBuffer,
                      logInitBuffer,
                      serverInitBuffer,
                      clientInitBuffer,
                      compInitBuffer;

    // Make sure that the various sections are commented to make the generated code a little
    // clearer.
    compInitDeclBuffer<< "// Declare all component initializers." << std::endl;
    sessionBuffer     << "// Declare component log session variables." << std::endl;
    filterBuffer      << "// Declare log filter level pointer variables." << std::endl;
    serverInitDeclBuffer << "// Declare server-side IPC API initializers." << std::endl;
    clientInitDeclBuffer << "// Declare client-side IPC API initializers." << std::endl;
    logInitBuffer     << "    // Initialize all log sessions." << std::endl;
    serverInitBuffer  << "    // Initialize all server-side IPC API interfaces." << std::endl;
    clientInitBuffer  << "    // Initialize all client-side IPC API interfaces." << std::endl;
    compInitBuffer    << "    // Schedule component initializers for execution by the event loop."
                      << std::endl;

    // iterate over the list of Component Instances,
    auto componentList = executable.ComponentInstances();
    for (auto i = componentList.crbegin(); i != componentList.crend(); i++)
    {
        const legato::ComponentInstance& componentInstance = *i;
        const legato::Component& component = componentInstance.GetComponent();

        const auto& name = component.CName();

        // Define the component's Log Session Reference variable.
        sessionBuffer  << "le_log_SessionRef_t " << name << "_LogSession;" << std::endl;

        // Define the component's Log Filter Level variable.
        filterBuffer   << "le_log_Level_t* " << name << "_LogLevelFilterPtr;" << std::endl;

        // Register the component with the Log Control Daemon.
        logInitBuffer  << "    " << name << "_LogSession = log_RegComponent(\""
                       << name << "\", &" << name << "_LogLevelFilterPtr);"
                       << std::endl;

        // For each of the component instance's server-side interfaces,
        for (const auto& mapEntry : componentInstance.ProvidedApis())
        {
            const auto& interface = mapEntry.second;

            // Skip this interface if it is marked for manual start.
            if (!interface.ManualStart())
            {
                // Declare the server-side interface initialization function.
                serverInitDeclBuffer << "void " << interface.InternalName()
                                     << "_StartServer(const char* serviceName);" << std::endl;

                // Call the server-side interface initialization function.
                serverInitBuffer << "    " << interface.InternalName() << "_StartServer(\""
                                 << interface.ServiceInstanceName() << "\");" << std::endl;
            }
        }

        // For each of the component instance's client-side interfaces,
        for (const auto& mapEntry : componentInstance.RequiredApis())
        {
            const auto& interface = mapEntry.second;

            // Skip this interface if we're only using the type definitions from this .api or it
            // is marked for manual start.
            if ((!interface.TypesOnly()) && (!interface.ManualStart()))
            {
                // Declare the client-side interface initialization function.
                clientInitDeclBuffer << "void " << interface.InternalName()
                                     << "_StartClient(const char* serviceName);" << std::endl;

                // Call the client-side interface initialization function.
                clientInitBuffer << "    " << interface.InternalName() << "_StartClient(\""
                                 << interface.ServiceInstanceName() << "\");" << std::endl;
            }
        }
    }

    // Now do the above for the default component.
    const legato::Component& defaultComponent = executable.DefaultComponent();

    {
        auto name = defaultComponent.CName();

        // Define the component's Log Session Reference variable.
        sessionBuffer  << "le_log_SessionRef_t " << name << "_LogSession;" << std::endl;

        // Define the component's Log Filter Level variable.
        filterBuffer   << "le_log_Level_t* " << name << "_LogLevelFilterPtr;" << std::endl;

        // Register the component with the Log Control Daemon.
        logInitBuffer  << "    " << name << "_LogSession = log_RegComponent(\""
                       << name << "\", &" << name << "_LogLevelFilterPtr);"
                       << std::endl;

    }

    // Queue up the component initializers to be called when the Event Loop starts.
    GenerateInitFunctionCalls(executable, compInitDeclBuffer, compInitBuffer);

    // Now that we have all of our subsections filled out, dump all of the generated code
    // and the static template code to the target output file.
    outputFile  << std::endl
                << "// Startup code for the executable '" << executable.OutputPath() << "'."
                << std::endl
                << "// This is a generated file, do not edit." << std::endl
                << std::endl
                << "#include \"legato.h\"" << std::endl
                << "#include \"../src/eventLoop.h\"" << std::endl
                << "#include \"../src/log.h\"" << std::endl
                << "#include \"../src/args.h\"" << std::endl

                << std::endl
                << std::endl

                << serverInitDeclBuffer.str() << std::endl
                << clientInitDeclBuffer.str() << std::endl

                << compInitDeclBuffer.str() << std::endl

                << sessionBuffer.str() << std::endl

                << filterBuffer.str() << std::endl
                << std::endl
                << std::endl

                << "int main(int argc, char* argv[])" << std::endl
                << "{" << std::endl
                << "    // Gather the program arguments for later processing." << std::endl
                << "    arg_SetArgs((size_t)argc, (char**)argv);" << std::endl
                << std::endl

                << logInitBuffer.str() << std::endl
                << "    // Connect to the log control daemon." << std::endl
                << "    // Note that there are some rare cases where we don't want the" << std::endl
                << "    // process to try to connect to the Log Control Daemon (e.g.," << std::endl
                << "    // the Supervisor and the Service Directory shouldn't)." << std::endl
                << "    // The NO_LOG_CONTROL macro can be used to control that." << std::endl
                << "    #ifndef NO_LOG_CONTROL" << std::endl
                << "        log_ConnectToControlDaemon();" << std::endl
                << "    #else" << std::endl
                << "        LE_DEBUG(\"Not connecting to the Log Control Daemon.\");" << std::endl
                << "    #endif" << std::endl
                << std::endl
                << "    LE_DEBUG(\"== Log sessions registered. ==\");" << std::endl
                << std::endl

                << "    // TODO: Load configuration." << std::endl
                << "    // TODO: Create configured memory pools." << std::endl
                << std::endl

                // Queue component initialization functions for execution by the event loop.
                << compInitBuffer.str() << std::endl
                << std::endl

                // Start server-side IPC interfaces now.  Any events that are caused by
                // this will get handled after the component initializers because those
                // are already on the event queue.
                << serverInitBuffer.str() << std::endl

                // Start client-side IPC interfaces now.  If there are any clients in
                // this thread that are bound to services provided by servers in this thread,
                // then at least we won't have the initialization deadlock of clients blocked
                // waiting for services that are yet to be advertised by the same thread.
                // However, until we support component-specific event loops and side-processing
                // of other components' events while blocked, we will still have deadlocks
                // if bound-together clients and servers are running in the same thread.
                << clientInitBuffer.str() << std::endl

                << "    LE_DEBUG(\"== Starting Event Processing Loop ==\");" << std::endl
                << "    le_event_RunLoop();" << std::endl
                << "    LE_FATAL(\"== SHOULDN'T GET HERE! ==\");" << std::endl
                << "}" << std::endl;

    // Add the generated file to the list of C source code files to be compiled into this
    // executable (as part of its "default" component).
    executable.AddCSourceFile(path);
}



//--------------------------------------------------------------------------------------------------
/**
 * Add to the build command-line link directives for the component libraries for all
 * sub-components of a given component instance and all components they are directly or
 * indirectly dependent on.
 */
//--------------------------------------------------------------------------------------------------
static void LinkComponent
(
    legato::Component& component,
    std::stringstream& commandLine
)
//--------------------------------------------------------------------------------------------------
{
    // Link the component.
    commandLine << " -l" << component.Name();

    // Link all the sub-components it depends on.
    for (auto& mapEntry : component.SubComponents())
    {
        legato::Component* componentPtr = mapEntry.second;

        if (componentPtr == NULL)
        {
            throw legato::Exception("Unresolved sub-component '" + mapEntry.first + "'"
                                    " of component '" + component.Name() + "'.");
        }

        LinkComponent(*componentPtr, commandLine);
    }
}



//--------------------------------------------------------------------------------------------------
/**
 * Add to the build command-line link directives for the component libraries for
 * a given component instance and all components it is directly or indirectly dependent on.
 */
//--------------------------------------------------------------------------------------------------
static void LinkComponentInstance
(
    legato::ComponentInstance& componentInstance,
    std::stringstream& commandLine
)
//--------------------------------------------------------------------------------------------------
{
    // Link all server-side APIs (because they'll call functions defined in the component lib).
    for (auto& mapEntry : componentInstance.ProvidedApis())
    {
        auto& interface = mapEntry.second;

        commandLine << " -l" << interface.Lib().ShortName();
    }

    // Link the component library and all its sub-components.
    LinkComponent(componentInstance.GetComponent(), commandLine);

    // Re-link all the async and manual-start server-side APIs (because there are functions
    // in there that the component will need to call).
    for (auto& mapEntry : componentInstance.ProvidedApis())
    {
        auto& interface = mapEntry.second;

        if (interface.IsAsync() || interface.ManualStart())
        {
            commandLine << " -l" << interface.Lib().ShortName();
        }
    }

    // Link all the client-side APIs (because they contain functions that the component calls).
    for (auto& mapEntry : componentInstance.RequiredApis())
    {
        auto& interface = mapEntry.second;

        // Skip this interface if we're only using the type definitions from this .api.
        if (!interface.TypesOnly())
        {
            const legato::Library& lib = interface.Lib();

            commandLine << " -l" << lib.ShortName();
        }
    }
}



//--------------------------------------------------------------------------------------------------
/**
 * Builds the source files in the executable's default component and links everything together to
 * create the executable file.
 */
//--------------------------------------------------------------------------------------------------
void ExecutableBuilder_t::Build
(
    const legato::Executable& executable
)
//--------------------------------------------------------------------------------------------------
{
    std::stringstream commandLine;
    std::string outputPath;
    const legato::Component& defaultComponent = executable.DefaultComponent();

    // If the executable file path is a relative path, then it is relative to the
    // executable output directory.
    if (! legato::IsAbsolutePath(executable.OutputPath()))
    {
        outputPath = m_Params.ExeOutputDir() + "/";
    }
    outputPath += executable.OutputPath();

    if (m_Params.IsVerbose())
    {
        std::cout << "Compiling and linking executable '" << outputPath << "'." << std::endl;
    }

    auto instanceList = executable.ComponentInstances();

    // Specify the compiler command and the output file path.
    commandLine << mk::GetCompilerPath(m_Params.Target()) <<" -o " << outputPath;

    // Turn on all warnings and treat them as errors.
    commandLine << " -Wall"
                << " -Werror";

    // If the executable is going to be debugged using gdb, turn off optimizations and turn
    // on debug symbols in the output.
    if (executable.IsDebuggingEnabled())
    {
        commandLine << " -g -O0";
    }

    // Add the CFLAGS to the command-line.
    commandLine << " " << m_Params.CCompilerFlags();

    // Add the include paths specified on the command-line.
    for (auto i : m_Params.InterfaceDirs())
    {
        commandLine << " -I" << i;
    }

    // Add the include paths specific to the default component.
    for (auto i : defaultComponent.IncludePath())
    {
        commandLine << " -I" << i;
    }

    // Define the component name, log session variable, and log filter variable for the default
    // component.
    commandLine << " -DLEGATO_COMPONENT=" << defaultComponent.CName();
    commandLine << " -DLE_LOG_SESSION=" << defaultComponent.Name() << "_LogSession ";
    commandLine << " -DLE_LOG_LEVEL_FILTER_PTR=" << defaultComponent.Name() << "_LogLevelFilterPtr ";

    // Define the COMPONENT_INIT for the default component.
    commandLine << " \"-DCOMPONENT_INIT=LE_CI_LINKAGE void "
                << mk::GetComponentInitName(defaultComponent) << "()\"";

    // Add all the default component's source files to the command-line (compile them now).
    // TODO: Change to pre-compile the .c files to .o files and then just include the .o files here.
    for (const auto& sourceFile : defaultComponent.CSourcesList())
    {
        commandLine << " \"" << sourceFile << "\"";
    }

    // Add the library output directory as a library search directory.
    for (auto instance : instanceList)
    {
        commandLine << " -L" << m_Params.LibOutputDir();
    }

    // Link with each component instance's component library and interface libraries,
    // as well as any component libraries for components it depends on.
    for (auto componentInstance : instanceList)
    {
        LinkComponentInstance(componentInstance, commandLine);
    }

    // Link with other libraries that are needed by the default component.
    for (const auto& lib : defaultComponent.LibraryList())
    {
        commandLine << " \"" << lib << "\"";
    }

    // Link with the Legato C runtime library.
    commandLine << " -L$LEGATO_ROOT/build/" << m_Params.Target() << "/bin/lib" << " -llegato";

    // Link with other libraries added to components included in this executable.
    for (const auto& i : instanceList)
    {
        auto libList = i.GetComponent().LibraryList();
        for (auto lib : libList)
        {
            commandLine << " -l" << lib;
        }
    }

    // Link with the real-time library, pthreads library, and the math library, just in case they're
    // needed too.
    // NOTE: Must link libm before libstdc++, or some math functions (like atan2()) won't be
    //       resolved properly .
    commandLine << " -lpthread -lrt -lm";

    if (executable.HasCppSources())
    {
        // If this is a C++ app, grab the C++ standard lib too.
        commandLine << " -lstdc++";
    }

    // Insert LDFLAGS on the command-line.
    commandLine << m_Params.LinkerFlags();

    // On the localhost, set the DT_RUNPATH variable inside the executable to include the
    // expected locations of the libraries needed.
    if (m_Params.Target() == "localhost")
    {
        commandLine << " -Wl,--enable-new-dtags"
                    << ",-rpath=\"\\$ORIGIN/../lib:" << m_Params.LibOutputDir()
                                                     << ":$LEGATO_ROOT/build/localhost/bin/lib\"";
    }
    // On embedded targets, set the DT_RUNPATH variable inside the executable to include the
    // expected location of libraries bundled in this application (this is needed for unsandboxed
    // applications).
    else
    {
        commandLine << " -Wl,--enable-new-dtags,-rpath=\"\\$ORIGIN/../lib\"";
    }

    if (m_Params.IsVerbose())
    {
        std::cout << std::endl << "$ " << commandLine.str() << std::endl << std::endl;
    }

    mk::ExecuteCommandLine(commandLine);
}


