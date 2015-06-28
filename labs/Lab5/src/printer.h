/**
 * @file      printer.h
 * @author    Jeramie Vens
 * @date      2015-02-13: Created
 * @date      2015-02-15: Last updated
 * @brief     A virtual printer for the print sever program
 * @copyright MIT License (c) 2015
 */
 
/*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*/

#ifndef PRINTER_H
#define PRINTER_H

#include "print_job.h"

#ifdef __cplusplus
extern "C" {
#endif 

/// Publically accessable printer object
typedef struct PRINTER_STRUCT* printer_t;
/// A internal representation of a printer device
struct PRINTER_STRUCT
{
	/// The name of this printer
	char* printer_name;
	/// The driver name (set by printer_install() function)
	const char* driver_name;
	/// The driver version (set by printer_install() function)
	const char* driver_version;
	/// Handle to the library object.  Ignore for Lab 5, we will need this in a future lab
	void* lib_handle;
	/**
	 * @brief      Uninstall this printer and clean up any resources it has allocated.
	 * @param      printer
	 *                  A pointer to this object
	 * @return     0 on success, <0 otherwise
	 */
	int (*uninstall)(printer_t printer);
	/**
	 * @brief      Print a job to the printer.
	 * @param      printer
	 *                 A pointer to this object
	 * @param      job
	 *                 The print job to print
	 * @return     0 on success, <0 otherwise
	 * example useage:
	 *   int error = printer->print(printer, my_job);
	 */
	int (*print)(printer_t, print_job_t*);
};

/**
 * @brief     Install the given printer and return a handle to it
 * @param     future
 *                 Handle to the library object.  Ignore for Lab 5, we will need this in a future lab.
 * @param     printer_name
 *                 A unique name to give the printer
 * @return    A printer object which can invoke print and uninstall methods.
 */
printer_t     printer_install          (void* future, char* printer_name);

#ifdef __cplusplus
}
#endif

#endif

