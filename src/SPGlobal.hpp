/*  SPMod - SourcePawn Scripting Engine for Half-Life
 *  Copyright (C) 2018  SPMod Development Team
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "spmod.hpp"

class PluginMngr;
class ForwardMngr;
class CvarMngr;
class Logger;

class SPGlobal : public ISPGlobal
{
public:
    #ifdef SP_LINUX
        static constexpr auto *sourcepawnLibrary = "sourcepawn.jit.x86.so";
    #elif defined SP_WINDOWS
        static constexpr auto *sourcepawnLibrary = "sourcepawn.jit.x86.dll";
    #endif

    SPGlobal() = delete;
    explicit SPGlobal(fs::path &&dllDir);
    ~SPGlobal()
    {
        #ifdef SP_POSIX
            dlclose(m_SPLibraryHandle);
        #else
            FreeLibrary(m_SPLibraryHandle);
        #endif
    }

    // ISPGlobal
    const char *getHome() const override;
    const char *getModName() const override;
    IPluginMngr *getPluginManager() const override;
    IForwardMngr *getForwardManager() const override;
    ICvarMngr *getCvarManager() const override;
    SourcePawn::ISourcePawnEnvironment *getSPEnvironment() const override;
    INativeMngr *getNativeManager() const override;
    ITimerMngr *getTimerManager() const override;
    IUtils *getUtils() const override;

    /**
     * @brief Returns home dir of SPMod.
     *
     * @return        Home dir.
     */
    const char *getHome() const override
    {
        return m_SPModDir.string().c_str();
    }

    /**
     * @brief Returns name of the mod.
     *
     * @return        Mod name.
     */
    const char *getModName() const override
    {
        return m_modName.c_str();
    }

    /**
     * @brief Returns SPMod plugin manager.
     *
     * @return        Plugin manager.
     */
    IPluginMngr *getPluginManager() const override
    {
        return reinterpret_cast<IPluginMngr *>(m_pluginManager.get());
    }

    /**
     * @brief Returns SPMod forward manager.
     *
     * @return                Forward manager.
     */
    IForwardMngr *getForwardManager() const override
    {
        return reinterpret_cast<IForwardMngr *>(m_forwardManager.get());
    }

    /**
    * @brief Returns SPMod cvar manager.
    *
    * @return                Cvar manager.
    */
    ICvarMngr *getCvarManager() const override
    {
        return reinterpret_cast<ICvarMngr *>(m_cvarManager.get());
    }
    /**
     * @brief Returns current SourcePawn environment.
     *
     * @return                SourcePawn environment.
     */
    SourcePawn::ISourcePawnEnvironment *getSPEnvironment() const override
    {
        return m_spFactory->CurrentEnvironment();
    }

    /**
     * @brief Returns SPMod native manager.
     *
     * @return              Native manager.
     */
    INativeMngr *getNativeManager() const override
    {
        return reinterpret_cast<INativeMngr *>(m_nativeManager.get());
    }

    /**
     * @brief Return SPMod timer manager.
     * 
     * @return              Timer manager.
     */
    ITimerMngr *getTimerManager() const override
    {
        return reinterpret_cast<ITimerMngr *>(m_timerManager.get());
    }

    /**
     * @brief Return SPMod menu manager.
     * 
     * @return              Timer manager.
     */
    IMenuManager *getMenuManager() const override
    {
        return reinterpret_cast<IMenuManager *>(m_menuManager.get());
    }

    /**
     * @brief Formats a string according to the SPMod format rules.
     *
     * @param buffer        Destination buffer.
     * @param length        Length of buffer.
     * @param format        Formatting string.
     * @param ctx           Plugin context.
     * @param params        Params list passed by native.
     * @param param            Index of param which contains first formatting argument.
     *
     * @return                Number of characters written.
     */
    unsigned int formatString(char *buffer,
                              std::size_t length,
                              const char *format,
                              SourcePawn::IPluginContext *ctx,
                              const cell_t *params,
                              std::size_t param) const override;

    // SPGlobal
    const auto &getPluginManagerCore() const
    {
        return m_pluginManager;
    }
    const auto &getForwardManagerCore() const
    {
        return m_forwardManager;
    }
    const auto &getCvarManagerCore() const
    {
        return m_cvarManager;
    }
    const auto &getLoggerCore() const
    {
        return m_loggingSystem;
    }
    const auto &getNativeManagerCore() const
    {
        return m_nativeManager;
    }
    const auto &getCommandManagerCore() const
    {
        return m_cmdManager;
    }
    const auto &getTimerManagerCore() const
    {
        return m_timerManager;
    }
    const std::unique_ptr<MenuManager> &getMenuManagerCore() const
    {
        return m_menuManager;
    }
    const auto &getScriptsDirCore()
    const auto &getUtilsCore() const
    {
        return m_utils;
    }
    const auto &getScriptsDirCore() const
    {
        return m_SPModScriptsDir;
    }
    const auto &getLogsDirCore() const
    {
        return m_SPModLogsDir;
    }
    const auto &getDllsDirCore() const
    {
        return m_SPModDllsDir;
    }

    void setScriptsDir(std::string_view folder);
    void setLogsDir(std::string_view folder);
    void setDllsDir(std::string_view folder);

private:
    void _initSourcePawn();

    fs::path m_SPModScriptsDir;
    fs::path m_SPModDir;
    fs::path m_SPModLogsDir;
    fs::path m_SPModDllsDir;
    std::unique_ptr<NativeMngr> m_nativeManager;
    std::unique_ptr<PluginMngr> m_pluginManager;
    std::unique_ptr<ForwardMngr> m_forwardManager;
    std::unique_ptr<CvarMngr> m_cvarManager;
    std::unique_ptr<Logger> m_loggingSystem;
    std::unique_ptr<CommandMngr> m_cmdManager;
    std::unique_ptr<TimerMngr> m_timerManager;
    std::unique_ptr<MenuManager> m_menuManager;
    std::unique_ptr<Utils> m_utils;
    std::string m_modName;
    SourcePawn::ISourcePawnFactory *m_spFactory;

    // SourcePawn library handle
#ifdef SP_POSIX
    void *m_SPLibraryHandle;
#else
    HMODULE m_SPLibraryHandle;
#endif
};

extern std::unique_ptr<SPGlobal> gSPGlobal;