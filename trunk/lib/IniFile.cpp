#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include "IniFile.h"

size_t IniFile::GetPrivateProfileStr( const char * lpAppName, const char * lpKeyName, const char * lpDefault, char * lpReturnedString, size_t nSize, const char * lpFileName )
{
	char buff[MAX_LINE_LENGTH];   
	char *ep;   
	char t_section[MAX_LINE_LENGTH];   
	char *ptr;   
	int tlen;   
	int len = strlen(lpKeyName);   

	FILE *fp = fopen(lpFileName,"r");   
	if( !fp ) return(0);   

	sprintf(t_section,"[%s]",lpAppName);    /* Format the section name */   
	tlen=strlen(t_section);   

	/*  Move through file 1 line at a time until a section is matched or EOF */   
	do   
	{   if( fgets(buff,MAX_LINE_LENGTH-1,fp) == NULL)   
	{   fclose(fp);   
		strncpy(lpReturnedString,lpDefault,nSize);        
		return(strlen(lpReturnedString));   
	}   
	}   
	while( strncasecmp(buff,t_section,tlen) );   

	/* Now that the section has been found, find the entry.   
	* Stop searching upon leaving the section's area. */   
	do   
	{      
		if ( fgets(buff,MAX_LINE_LENGTH-1,fp) == NULL)   
		{   fclose(fp);   
		strncpy(lpReturnedString,lpDefault,nSize);        
		return(strlen(lpReturnedString));   
		}   
	}  while( strncasecmp(buff,lpKeyName,len) );   

	fclose(fp);  

	ep = strchr(buff,'=');    /* Parse out the equal sign */  
	if (ep == NULL)  
	{  
		strncpy(lpReturnedString,lpDefault,nSize);       
		return(strlen(lpReturnedString));  
	}  
	ep++;  

	/* remove leading spaces*/  
	while (*ep && (isspace(*ep) || *ep == 10))  
		ep++;  
	if (ep == NULL)  
	{  
		strncpy(lpReturnedString,lpDefault,nSize);       
		return(strlen(lpReturnedString));  
	}  

	/* remove trailing spaces*/  
	ptr = ep;  
	while(*ptr) // go to the end, point to a NULL  
		ptr++;  

	ptr--;  
	while (ptr > ep)  // backup and put in nulls if there is a space  
	{  
		if (isspace(*ptr) || *ep == 10)  
		{  
			*ptr = 0;  
			ptr--;  
		}  
		else  
			break;  
	}  

	/* Copy up to nSize chars to buffer */  
	strncpy(lpReturnedString,ep,nSize - 1);  

	lpReturnedString[nSize] = '\0';  
	return(strlen(lpReturnedString));
}
