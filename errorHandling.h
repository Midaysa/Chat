/*
 * errorHandling.h
 *
 *  Created on: Jan 20, 2016
 *      Author: francisco
 */

#ifndef ERRORHANDLING_H_
#define ERRORHANDLING_H_

/*extern const char *mkfifoError;
extern const char *selectError;
extern const char *openError;
*/
char* getError(char* errorMessage,int line, char* file);

#endif /* ERRORHANDLING_H_ */
