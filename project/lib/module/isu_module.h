/**
 * @addtogroup libraries
 * @{
 */
/**
 * @addtogroup libmodule Runtime Loadable Module Library
 * @brief     Library allowing for the loading of modules at runtime
 * @details   This library allows for loading of compiled modules at runtime.
 *            The modules should be compiled as shared library files and must
 *            export the install() function which takes no parameters and
 *            returns a pointer.  The module may optionally export a uninstall()
 *            fuction which takes no parameters and returns nothing which will
 *            be called at uninstall time.
 *
 *            The install function returnes a pointer which should be an object
 *            containing function pointers to the functions the module supports.
 * @{
 */
/**
 * @file      isu_module.h
 * @author    Jeramie Vens
 * @date      3/16/15 - Created
 * @date      3/17/15 - Added documentation
 * @brief     Include file for the runtime loadable module library.
 */


#ifndef ISU_MODULE_H
#define ISU_MODULE_H

#include "common/isu_types.h"

/**
 * @brief     Install a new module into the system
 * @param     name
 *                 The path name to the module file.
 * @return    The return of the modules install() function if the module was
 *            successfully loaded, NULL if the module could not be loaded.
 * @details   The isu_moduel_install() function will attempt to install a
 *            runtime loadable libary.  The module must be compiled as a shared
 *            library and MUST support an install() function which returns a
 *            pointer.  It may optionally export an uninstall() function which
 *            is called by isu_module_uninstall().
 */
void* isu_module_install(char * name);

/**
 * @brief     Uninstall a currently installed module.
 * @param     name
 *                 The name of the module.  This must be the same name that was
 *                 passed to isu_module_install()
 * @return    ISU_RET_EPARAM:
 *                 The module was not found in the list of installed modules.
 * @return    ISU_RET_SUCCESS:
 *                 The module was successfully uninstalled.
 * @details   isu_module_uninstall() searches through the list of installed
 *            modules and uninstalles the given module if it is found.  It will
 *            attempt to call the modules uninstall() function if one exists.
 */
isu_return_t isu_module_uninstall(char * name);

/**
 * @brief     Uninstall all currently installed modules.
 * @return    ISU_RET_SUCCESS:
 *                 All installed modules successfully uninstalled.
 * @return    Other Error:
 *                 Something went wrong during the uninstall of one of the
 *                 modules.
 * @details   This is the same as calling isu_module_uninstall() on each
 *            installed module.
 */
 isu_return_t isu_module_uninstall_all(void);
#endif

/// @}
/// @}

