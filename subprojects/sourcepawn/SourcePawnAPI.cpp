/*
 *  Copyright (C) 2018-2020 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  SPMod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SPMod is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SPMod.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ExtMain.hpp"

std::unique_ptr<SourcePawnAPI> gSPAPI;

SourcePawnAPI::SourcePawnAPI(const fs::path &libraryDir)
{
    fs::path libraryName = libraryDir / SourcePawnAPI::sourcepawnLibrary;

#if defined SP_POSIX
    auto libraryHandle = std::unique_ptr<void, std::function<void(void *)>>(
        dlopen(libraryName.c_str(), RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND), [](void *ptr) {
            dlclose(ptr);
        });
#else
    HMODULE libraryHandle = LoadLibrary(libraryDir.string().c_str());
#endif

    if (!libraryHandle)
        throw std::runtime_error(libraryName.string() + " Failed to open SourcePawn library");

#if defined SP_POSIX
    auto getFactoryFunc =
        reinterpret_cast<SourcePawn::GetSourcePawnFactoryFn>(dlsym(libraryHandle.get(), "GetSourcePawnFactory"));
#else
    auto getFactoryFunc =
        reinterpret_cast<SourcePawn::GetSourcePawnFactoryFn>(GetProcAddress(libraryHandle, "GetSourcePawnFactory"));
#endif

    if (!getFactoryFunc)
    {
#if defined SP_WINDOWS
        FreeLibrary(libraryHandle);
#endif
        throw std::runtime_error("Cannot find SourcePawn factory function");
    }

    SourcePawn::ISourcePawnFactory *SPFactory = getFactoryFunc(SOURCEPAWN_API_VERSION);
    if (!SPFactory)
    {
#if defined SP_WINDOWS
        FreeLibrary(libraryHandle);
#endif
        throw std::runtime_error("Wrong SourcePawn library version");
    }

    m_SPLibraryHandle = std::move(libraryHandle);
    m_spFactory = SPFactory;
    m_spFactory->NewEnvironment();
    getSPEnvironment()->APIv2()->SetJitEnabled(true);
}

#if defined SP_WINDOWS
SourcePawnAPI::~SourcePawnAPI()
{
    FreeLibrary(m_SPLibraryHandle);
}
#endif

SourcePawn::ISourcePawnEnvironment *SourcePawnAPI::getSPEnvironment() const
{
    return m_spFactory->CurrentEnvironment();
}