/**
 * @addtogroup libmodule
 * @{
 */
/**
 * @file      isu_module.c
 * @author    Jeramie Vens
 * @date      03/16/15 - Created
 * @date      03/17/15 - Documented, added uninstall ability
 * @brief     Handle runtime loadable modules.
 * @details   This file contains the source code and list of currently loaded
 *            modules.
 */
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include "isu_module.h"
#include "llist/isu_llist.h"
#include "common/isu_error.h"
#include "common/isu_types.h"

/**
 * A node in the module list.
 */
struct MODULE_STRUCT
{
	/// The handle to the module returned from dlopen()
	void* handle;
	/// The name of the module as passed to isu_module_install()
	char* name;
};

/// The list of currently installed modules
static isu_llist_t mod_list = NULL;

void* isu_module_install(char * name)
{
	isu_return_t rv;
	void* handle;
	char* error;
	struct MODULE_STRUCT *mod;
	void* (*install)(void);
	
	isu_print(PRINT_DEBUG, "Installing module %s", name);
	
	// open the module file
	handle = dlopen (name, RTLD_NOW);
	if (!handle) {
        isu_print(PRINT_ERROR, "Failed to open module: %s", dlerror());
        return NULL;
    }
	
	// get the install function
	install = dlsym(handle, "install");
	 if ((error = dlerror()) != NULL)  {
        isu_print(PRINT_ERROR, "Failed to read module: %s", error);
        dlclose(handle);
        return NULL;
    }
	
	// create a new module structure object
	mod = calloc(1, sizeof(struct MODULE_STRUCT));
	if(!mod)
	{
		isu_print(PRINT_ERROR, "Could not allocate memory for new module");
		dlclose(handle);
		return NULL;
	}
	
	// copy the module information into the module structure object
	mod->handle = handle;
	mod->name = malloc(strlen(name)+1);
	if(!mod->name)
	{
		isu_print(PRINT_ERROR, "Could not allocate memory for module name");
		free(mod);
		dlclose(handle);
		return NULL;
	}
	strcpy(mod->name, name);
	
	// if the mod_list doesn't exits create it
	if(!mod_list)
	{
		mod_list = isu_llist_create();
		if(!mod_list)
		{
			isu_print(PRINT_ERROR, "Could not create module list");
			free(mod->name);
			free(mod);
			dlclose(handle);
			return NULL;
		}
	}
	
	// push the module onto the list
	if(rv = isu_llist_push(mod_list, mod, ISU_LLIST_TAIL))
	{
		isu_print(PRINT_ERROR, "Could not add module to list");
		free(mod->name);
		free(mod);
		dlclose(handle);
		return NULL;
	}
	
	isu_print(PRINT_DEBUG, "Successfully installed module");
	// call install() and return the result
	return install();
}

/**
 * @brief     Uninstall the given module object
 * @private
 * @param     mod
 *                 The module to uninstall
 * @return    ISU_RET_SUCCESS
 */
isu_return_t isu_module_uninstall_mod(struct MODULE_STRUCT *mod)
{
	void (*uninstall)(void);
	isu_llist_remove(mod_list);
	
	// attempt to get the uninstall function
	uninstall = dlsym(mod->handle, "uninstall");
	if (dlerror() == NULL)  {
	    isu_print(PRINT_DEBUG, "Calling modules uninstall() fuction");
	    uninstall();
	}
	
	dlclose(mod->handle);
	isu_print(PRINT_DEBUG, "Successfully uninstalled module %s", mod->name);
	free(mod->name);
	free(mod);
	return ISU_RET_SUCCESS;
}

isu_return_t isu_module_uninstall(char * name)
{
	struct MODULE_STRUCT *mod;
	isu_return_t rv;
	isu_print(PRINT_DEBUG, "Uninstalling module %s", name);
	for_each(mod, mod_list)
	{
		if(strcmp(mod->name, name) == 0)
		{
			rv = isu_module_uninstall_mod(mod);
			if(rv)
			{
				isu_print(PRINT_ERROR, "Failed to uninstall module");
				return rv;
			}
			if(isu_llist_count(mod_list) == 0)
			{
				isu_llist_destroy(mod_list);
				mod_list = NULL;
			}
			return ISU_RET_SUCCESS;
		}
	}
	isu_print(PRINT_NOTICE, "Unable to find mod %s installed", name);
	return ISU_RET_EPARAM;
}

isu_return_t isu_module_uninstall_all(void)
{
	struct MODULE_STRUCT *mod;
	isu_return_t rv, my_rv = ISU_RET_SUCCESS;
	if(!mod_list)
	{
		isu_print(PRINT_DEBUG, "No modules installed");
		return ISU_RET_SUCCESS;
	}
	for_each(mod, mod_list)
	{
		rv = isu_module_uninstall_mod(mod);
		if(rv)
		{
			isu_print(PRINT_ERROR, "Failed to uninstall module %s", mod->name);
			my_rv = rv;
		}
	}
	isu_print(PRINT_DEBUG, "Finished uninstalling all modules");
	return my_rv;
}

/// @}

