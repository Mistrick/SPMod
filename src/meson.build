if build_machine.system() == 'linux'
    add_project_link_arguments('-ldl', language : [ 'c', 'cpp'])
elif build_machine.system() == 'windows'
    linkArgs = [
        '/EXPORT:GiveFnptrsToDll=_GiveFnptrsToDll@8',
        '/SECTION:.data,RW'
    ]

    add_project_link_arguments(linkArgs, language : [ 'c', 'cpp'])
endif

sourceFiles = files('h_export.cpp',
                    'engine_api.cpp',
                    'dllapi.cpp',
                    'meta_api.cpp',
                    'SPGlobal.cpp',
                    'RehldsApi.cpp',
                    'CmdSystem.cpp',
                    'ForwardSystem.cpp',
                    'PlayerSystem.cpp',
                    'LoggingSystem.cpp',
                    'CvarSystem.cpp',
                    'TimerSystem.cpp',
                    'MenuSystem.cpp',
                    'ExtensionSystem.cpp',
                    'ValveInterface.cpp',
                    'UtilsSystem.cpp',
                    'NativeProxy.cpp',
                    'EngineFuncs.cpp',
                    'MetaFuncs.cpp',
                    'Edict.cpp')

shared_library('spmod_mm', sourceFiles, include_directories : includeDirs, gnu_symbol_visibility : 'hidden')
