/**
 * @file UIRBcore_Version.h
 * @brief Contains version information for the UIRBcore library.
 *
 * This header file is auto-generated during the installation process and defines the version number
 * of the UIRBcore library. The version is represented using the major, minor, and patch
 * components along with a human-readable string.
 *
 * @details
 * - Version format: MAJOR.MINOR.PATCH or MAJOR.MINOR.PATCH-COMMITS-gCOMMIT_HASH
 * - The @ref UIRB_CORE_LIB_VER_STR macro provides the complete version as a string.
 * - Individual macros (@ref UIRB_CORE_LIB_MAJOR, @ref UIRB_CORE_LIB_MINOR, etc.) allow programmatic access
 *   to each version component.
 *
 * @note
 * - Ensure the UIRBcore library is included before using this file.
 * - Do not modify this file manually; it is automatically generated and may be overwritten.
 * - The script retrieves the version from the latest Git tag in the repository. If no tags exist,
 *   the script will fall back to the commit hash.
 * 
 * @warning
 * - This file is regenerated during every installation process. Changes to this file will be lost.
 * 
 * @author 
 * Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * @copyright Copyright (c) 2024 Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * MIT License
 * 
 * Copyright (c) 2024 Djordje Mandic (https://linktr.ee/djordjemandic)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef UIRBcore_Version_h
#define UIRBcore_Version_h

#if defined(UIRB_CORE_LIB) || defined(__DOXYGEN__)

    #if defined(UIRB_CORE_LIB_VER_STR)
        #undef UIRB_CORE_LIB_VER_STR
    #endif  // defined(UIRB_CORE_LIB_VER_STR)

    /**
     * @def UIRB_CORE_LIB_VER_STR
     * @brief Full version string of the UIRBcore library.
     * 
     * This macro defines a string representing the full version of the UIRBcore library.
     * The version format is:
     * - `MAJOR.MINOR.PATCH` (e.g., "1.0.0")
     * - Or optionally prefixed with `v`: `vMAJOR.MINOR.PATCH` (e.g., "v1.0.0")
     * - For additional commits, the format includes commit details: 
     *   `MAJOR.MINOR.PATCH-COMMITS-g<COMMIT_HASH>` or `vMAJOR.MINOR.PATCH-COMMITS-g<COMMIT_HASH>`
     *   (e.g., "1.0.0-2-gabc1234" or "v1.0.0-2-gabc1234").
     * 
     * It is primarily used for logging and display purposes.
     */
    #define UIRB_CORE_LIB_VER_STR "v1.0.0"

    
    #if defined(UIRB_CORE_LIB_MAJOR)
        #undef UIRB_CORE_LIB_MAJOR
    #endif  // defined(UIRB_CORE_LIB_MAJOR)

    /**
     * @def UIRB_CORE_LIB_MAJOR
     * @brief Major version number of the UIRBcore library.
     * 
     * The major version number indicates significant changes in the library, such as
     * backward-incompatible API updates or major new functionality. Incrementing this
     * number signals a major release.
     */
    #define UIRB_CORE_LIB_MAJOR (1)
    
    #if defined(UIRB_CORE_LIB_MINOR)
        #undef UIRB_CORE_LIB_MINOR
    #endif  // defined(UIRB_CORE_LIB_MINOR)

    /**
     * @def UIRB_CORE_LIB_MINOR
     * @brief Minor version number of the UIRBcore library.
     * 
     * The minor version number represents new features or enhancements that are
     * backward-compatible with previous versions. Incrementing this number signals a
     * minor release.
     */
    #define UIRB_CORE_LIB_MINOR (0)
    
    #if defined(UIRB_CORE_LIB_PATCH)
        #undef UIRB_CORE_LIB_PATCH
    #endif  // defined(UIRB_CORE_LIB_PATCH)

    /**
     * @def UIRB_CORE_LIB_PATCH
     * @brief Patch version number of the UIRBcore library.
     * 
     * The patch version number indicates backward-compatible bug fixes or minor updates
     * that do not introduce new features. Incrementing this number signals a patch release.
     */
    #define UIRB_CORE_LIB_PATCH (0)

#else  // defined(UIRB_CORE_LIB)

    #warning "UIRBcore library should be included first."

#endif  // defined(UIRB_CORE_LIB)

#endif  // UIRBcore_Version_h
