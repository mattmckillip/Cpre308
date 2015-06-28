/**
 * @file	SMASH_Helper.h
 * @author	Matt McKillip and Lee Robinson
 * @date	4/08/2015 - Created
 * @brief	Header file for the helper file for SMASH shell
 */
 

/**
 * @brief	constructs a new main memory object
 * @param	a
 *				The const char we are testing if it starts with another const char
 * @param	b
 *				The cons char we are testing if another const char starts with
 * 			
 * @return	1 if a starts with b, 0 if it does not
 */
int starts_with(const char *a, const char *b);

/**
 * @brief	Prints the usage for the SMASH shell to the user
 *
 */
void printUsageMessage();

/**
 * @brief	Used as a parameter for scandir to determine which files to select
 * @param	entry
 *				The directory we are scanning
 * 			
 * @return	1 if valid file, 0 if not
 */
int file_select(const struct direct *entry);

/**
 * @brief	Used as a parameter for scandir to sort files by their name
 * @param	file1
 *				The first file to compare
 * @param	file2
 *				The second file to compare
 * 			
 * @return	which file comes first alphabetically
 */
int nameSort(const struct dirent** file1, const struct dirent** file2);

/**
 * @brief	Scans character array line by line (similar to fgets)
 * @param	s
 *				The character array to scan
 * @param	n
 *				The size of the array
 * @param	strp
 *				Pointer for the array
 * 			
 * @return	the current line of the character array, null if end of array
 */
char *sgets(char *s, int n, const char **strp);
