/*
 *  Copyright (C) 2016 Sauro Menna
 *
 *	This file is part of gcsortgentestcase.
 *
 *  GCSORT is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GCSORT is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GCSORT.  If not, see <http://www.gnu.org/licenses/>.

*/
// 
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <stdint.h>
#ifdef _MSC_VER
	#include <crtdbg.h>
#endif

#if	defined(__GNUC__) && !defined(__MINGW32__) && !defined(__MINGW64__)
	#define _strdup  strdup
#endif


#include <libcob.h>

#include "libgcsort.h"
#include "gcsort_gentestcase.h"
#include "gencfg.h"
#include "gencob4sort.h"
#include "gencob4check.h"
#include "genscript.h"

char szFileName[80];
//unsigned char filestatus[3];
int  opt;

/* Module structure pointer */
//
static cob_module	*module = NULL;
/* Global variable pointer */
cob_global		*cob_glob_ptr;

char szAllChar[] = {'0','1','2','3','4','5','6','7','8','9',
	               'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
				   'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
				   '!','�','$','%','&','/','(',')','=','?','^','*','�','�','@','#','[',']','{','}'};
char szCharUpp[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
char szCharLow[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
char szChar[]	 = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	                'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
char szNum[]     = {'0','1','2','3','4','5','6','7','8','9'};
char szSign[]    = {'+','-'};


int main(int argc, char* argv[])
{
	struct params_t* pParams;
	struct key_t*   pKey[100];
	pParams= (struct params_t* ) malloc(sizeof(struct params_t));
	cob_init(argc, argv);
	//
	cob_module_enter(&module, &cob_glob_ptr, 0);


	pParams->nLenMax=0;
	pParams->nLenMin=0;
	pParams->nMaxFields=-1;
	pParams->nNumRec=0;
	pParams->byteorder=0;
	pParams->nNumKeys=0;


	if (argc < 2) {
		fprintf(stderr,"\nFile cfg not found\n");
		//Usage();
		fprintf(stdout,"________________________________________________________________________\n");
		fprintf(stdout,"gcsort_gentestcase Version \n"); 
		fprintf(stdout,"Copyright (C) 2009-2016 Sauro Menna\n");
		fprintf(stdout,"________________________________________________________________________\n");
		fprintf(stdout,"gcsort_gentestcase. Nothing to do.\n");
		fprintf(stdout,"Usage: gcsort_gentestcase <file name config>\n");
		fprintf(stdout,"                       Execute command\n");
		fprintf(stdout,"or   : gcsort_gentestcase --gen \n");
		fprintf(stdout,"                       Create file config <example_gen.txt> with all entries\n");
		fprintf(stdout,"or   : gcsort_gentestcase --help\n");
		fprintf(stdout,"                       Print help\n");
		exit(-1);
	}
	if (strcmp(argv[1], "--gen") == 0) {
		CreateExampleFileConfig();
		exit(0);
	}

	if (strcmp(argv[1], "--help") == 0) {
		Usage();
		exit(0);
	}

	read_fileCFG( argv[1] , pParams, pKey);

	if (check_param(pParams, pKey ) == 0) {
		print_paramCFG( stdout, pParams, pKey);
		generate_file(pParams, pKey );
		generate_Takefile(pParams, pKey);
		Sort_Fields(pParams, pKey);
		generate_CobfileCheckFile(pParams, pKey);
		generate_CobfileSortFile(pParams, pKey );
		gen_script_win(pParams, pKey );
		gen_script_linux(pParams, pKey );
	}
	destroy_paramCFG ( pParams, pKey );

	cobtidy();
	
#ifdef _DEBUG
	#ifdef _MSC_VER
		_CrtDumpMemoryLeaks();
	#endif
#endif
//   

	return 0;
}

void gen_setAttrib ( cob_field_attr *attrArea, int type, int nLen)
{
// fix value for single type of field
	switch (type) {
        case COB_TYPE_ALPHANUMERIC_ALL:
        case COB_TYPE_ALPHANUMERIC:
            attrArea->digits = 0;
            attrArea->scale = 0;
            break;
	    case COB_TYPE_NUMERIC_BINARY:
            if (nLen <= 2)
                attrArea->digits = 4;   //        
            if ((nLen > 2) && (nLen <= 4))
                attrArea->digits = 9;   //        
            if (nLen > 4) 
                attrArea->digits = 18;   //       
            attrArea->scale = 0;
            break;
	    case COB_TYPE_NUMERIC_DOUBLE:
            attrArea->digits = 34;
            attrArea->scale = 17;
            attrArea->flags  = attrArea->flags | COB_FLAG_IS_FP | COB_FLAG_HAVE_SIGN;
            break;
	    case COB_TYPE_NUMERIC_FLOAT:
            attrArea->digits = 15;
            attrArea->scale = 8;
           attrArea->flags  = attrArea->flags | COB_FLAG_IS_FP | COB_FLAG_HAVE_SIGN;
            break;
        case COB_TYPE_NUMERIC_PACKED:
             if (nLen <= 1) 
                attrArea->digits = nLen*2;
             else
             {
                if (nLen % 2 == 0)
    		    	attrArea->digits = (nLen*2)-1; //(nLen-1)*2;
			    else
	    	    	attrArea->digits = (nLen*2)-1;
             }
            attrArea->scale = 0;
    	    attrArea->flags  = attrArea->flags | COB_FLAG_HAVE_SIGN;
            break;
        case COB_TYPE_NUMERIC_DISPLAY:
            attrArea->digits = 0;
            attrArea->scale = 0;
            break;
	}

    return;//
}

cob_field* gen_cob_field_create ( void )
{
	cob_field       *field_ret;
	cob_field_attr	*attrArea;
	attrArea = (cob_field_attr*) malloc(sizeof(cob_field_attr));
	field_ret = (cob_field*)malloc(sizeof(cob_field));
	field_ret->attr = attrArea;
    field_ret->data = NULL;

	return field_ret;
}
void gen_cob_field_set (cob_field* field_ret, int type, int digits, int scale, int flags, int nLen)
{
	cob_field_attr	*attrArea;
	attrArea = (cob_field_attr*)field_ret->attr;
	attrArea->type   = type;
	attrArea->digits = digits;
	attrArea->scale  = scale;
	attrArea->flags  = flags;
	attrArea->pic    = NULL;
	field_ret->size = nLen;

    gen_setAttrib ( attrArea, type, nLen); // Fix value

	return ;
}
void gen_cob_field_destroy ( cob_field* field_ret, int nData)
{
	if (field_ret!=NULL) {
		if (field_ret->attr!=NULL)
				free((void*)field_ret->attr); 
		if ((nData == ALLOCATE_DATA) && (field_ret->data!=NULL))
				free((void*)field_ret->data); 
		free(field_ret);  
	}
}
int gen_set_area (cob_file* file, unsigned char* szBuf, int nLen )
{
// set area data
	memcpy(file->record->data, szBuf, nLen);
    if (file->variable_record!= NULL) { // format == FILE_TYPE_VARIABLE){
		file->record->size = nLen;
		cob_set_int(file->variable_record, (int)nLen);
	}
	else
	{
		file->record->size = nLen;
	}
	return 0 ;
}


int check_param  ( struct params_t* params, struct key_t** pKey ) 
{
	// controllare sovrapposizione

	int nErr = 0;
	int i,n, nCheckKey;
	int nSeq[20];
	int nLenSeq=20;
	fprintf(stdout,"Start - Check config file\n");
	if ((params->byteorder != 0) && (params->byteorder != 1)){
		fprintf(stderr," Error byteorder value %d  - 0 Ntive, 1 BigEndian\n", params->byteorder);
		nErr++;
	}
	if (params->nLenMax < params->nLenMin){
		fprintf(stderr," Error len value len min/max %05d - %05d\n", params->nLenMin, params->nLenMax);
		nErr++;
	}
	if (params->nMaxFields == -1){
		fprintf(stderr," Error fields not specied\n");
		nErr++;
	}
	if (params->nNumRec == 0){
		fprintf(stderr," Error number of records\n");
		nErr++;
	}
	if (strlen(params->PathGen) == 0){
		fprintf(stderr," Error pathgen (Pathname for file output) not found \n");
		nErr++;
	}
	if (strlen(params->PathSrc) == 0){
		fprintf(stderr," Error pathsrc (Pathname for source Cobol) not found \n");
		nErr++;
	}
	if (strlen(params->PathTake) == 0){
		fprintf(stderr," Error pathtake (Pathname for file TAKE ) not found \n");
		nErr++;
	}
	if (strlen(params->PgmCheckData) == 0){
		fprintf(stderr," Error pgmcheckdata (Program name check data generated) not found \n");
		nErr++;
	}
	if (strlen(params->PgmCheckSort) == 0){
		fprintf(stderr," Error pgmchecksort (Program name check data sorted) not found \n");
		nErr++;
	}
	if (strlen(params->szFileName) == 0){
		fprintf(stderr," Error pgmchecksort (Program name check data sorted) not found \n");
		nErr++;
	}

	if (strlen(params->ScriptName) == 0) {
		fprintf(stderr," Error script (Name of script ) not found \n");
		nErr++;
	}

	if (strlen(params->PgmCheckSort) == 0){
		fprintf(stderr," Error pgmchecksort (Program name check data sorted) not found \n");
		nErr++;
	}


	// check sequence
	n=-1;
	nCheckKey=0;
	for (i=0; i < nLenSeq;i++)
		nSeq[i] = -1;
	for (i=0; i <= params->nMaxFields; i++) {
		if (pKey[i]->iskey == 0) 
			continue;
		n++;
		nSeq[n] = pKey[i]->seqkey;
	}
	i=0;
	for (n=1; n <= params->nNumKeys;n++) {
		for (i=0; i <= params->nMaxFields;i++) {
			if(nSeq[i] == n) 
				nCheckKey++;
		}
	}
	if (nCheckKey != params->nNumKeys) {
		fprintf(stderr," Error pgmchecksort - Order of sequence key (seqkey) is not strictly progressive. Start from 1 and add 1.\n");
	    fprintf(stderr,"            Sequence found : ");
		for (i=0; i < nCheckKey;i++) {
			fprintf(stderr,"%05d - ", nSeq[i]);
		}
		fprintf(stderr,"\n");
		nErr++;
	}

	if (nErr> 0) {
		fprintf(stdout,"Start - Check config file   KO \n");
		fprintf(stdout,"        Terminate with errors \n");
		return 1;
	}
	else
		fprintf(stdout,"Start - Check config file   OK \n");


	return 0;
}

int Sort_Fields ( struct params_t* params, struct key_t** pKey ) {
	int i=0;
	qsort(pKey, params->nMaxFields+1, sizeof(struct key_t**), compare4qsort);  // check record position
	return 0;
}

int compare4qsort (const void *first, const void *second) {
	struct key_t**  pKey1;
	struct key_t**  pKey2;
	pKey1 = (struct key_t**)first;
	pKey2 = (struct key_t**)second;
	
	if ((*pKey1)->pos < (*pKey2)->pos)
		return -1;
	if ((*pKey1)->pos > (*pKey2)->pos)
		return 1;
	
	return 0;
}

void Usage (void) 
{
	printf("gcsort_gentestcase	Help\n");
	printf("    Usage with file parameters   : gcsort <configfile>\n");
	printf("                <configfile> is file structure definition                               \n");
	printf("___________________________________________________________________________________\n");
	printf("  Definitions for environment variables script\n");
	printf("  scripthead_start    : Tag for start definition for global variables environment\n");
	printf("  <rows>              : <rows> = lines insert into file script\n");
	printf("                              example: export LD_LIBRARY_PATH=/usr/local/lib\n");
	printf("  scripthead_end      : Tag for end definition for global variables environment\n");
	printf("___________________________________________________________________________________\n");
	printf("  Definitions for parameters for GCSORT\n");
	printf("  gcsort_top_start    : Tag for start definition for environment variable OCsort\n");
	printf("  <rows>              : <rows> = lines insert before execution of OCsort\n");
	printf("                              example: GCSORT_MEMSIZE=512000000\n");
	printf("  gcsort_top_end    : Tag for start definition for environment variable OCsort\n");
	printf("___________________________________________________________________________________\n");
	printf("  Definitions for parameters for TAKE file GCSORT\n");
	printf("  gcsort_bottom_start : Tag for start definition commands after SORT command\n");
	printf("  <rows>              : <rows> = You can insert command Outrec, Inrec, SumField, OutFil\n");
	printf("                              example: OUTREC(1,10,30,80)\n");
	printf("  gcsort_bottom_start : Tag for start definition commands after SORT command\n");
	printf("___________________________________________________________________________________\n");
	printf("                        File definition\n");
	printf("\n");
	printf("  Config file for file generator\n");
	printf("Parameter                   Value \n");
	printf("script=<script name>   : <script name> : name of script for execution\n");
	printf("pgmcheckdata=<CBLcheck>: <CBLcheck>    : Program name cobol for check file generated\n");
	printf("pgmchecksort=<CBLsort> : <CBLsort>     : Program name cobol for check file sorted\n");
	printf("pathgen=<pathgen>      : <pathgen>     : Pathname for file generated by gcsort_gentestcase\n");
	printf("pathsrc=<pathsrc>      : <pathsrc>     : Pathname for source cobol generated\n");
	printf("pathtake=<pathtake>    : <pathtake>    : Pathname for file TAKE used by gcsort\n");
	printf("pathbatsh=<pathbatsh>  : <pathbatsh>   : Pathname for script (Windows/Linux> for execution\n");
	printf("filename=<filename>    : <filename>    : File name output \n");
	printf("organization=<org>     : <org>         : LS = line sequential, SQ = sequential\n");
	printf("                                       : RL = relative       , IX = indexed   \n");
	printf("record= <rectype>      : <rectype>     : type  F = fixed, V = variable\n");
	printf("lenmin=<lenmin>        : <lenmin>      : min length record. For Fixed lenmin = lenmax\n");
	printf("lenmax=<lenmax>        : <lenmax>      : max length record.\n");
	printf("byteorder=<byteorder>  : <byteorder>   : 0=native, 1=Bigendian\n");
	printf("numrec=<numrec>        : <numrec>      : Records number for generation\n"); 
	printf("___________________________________________________________________________________\n");
	printf("                        Field definition\n");
	printf("___________________________________________________________________________________\n");
	printf("pos=<pos>              : <pos>         : position of field (first position is 1)\n");
	printf("len=<len>              : <len>         : length of field \n");
	printf("type=<typeSrt>         : <typeSrt>     : type of field (CH,BI,FI,FL,PD,ZD) / (PR,FX)\n");
	printf("                             CH = Character, BI = Binary unsigned, FI = Fixed (Binary signed)\n");
	printf("                             PD = Packed, ZD = Zoned signed\n");
	printf("type=<typeNew>         : <typeNew>     : PR Progressive (Record Number), len define length of field.\n");
	printf("                                         FX = Fixed value\n");
	printf("value=<value>          : <value>       : literal. This option must be defined only for typer=FX.\n");
	printf("iskey=<iskey>          : <iskey>       : 'yes' if field is part of key\n");
	printf("seqkey=<nn>            : <nn>          : progressive sequence of field for key\n");
	printf("order=<X>              : <X>           : A for Ascending, D for Descending\n");
	printf("___________________________________________________________________________________\n");
	return;
}


void SetKeyForSort(struct params_t* params,  struct key_t** pKey, char* pBuf)
{
	char szKey[256];
	char szBuf[256];
    int n,i;
	if (params->nNumKeys > 0) {
		n=1;
		i=0;
		memset(szBuf, 0x20, sizeof(szBuf));
		do {
			if (pKey[i]->iskey == 1) {
				if (pKey[i]->seqkey == n) {
					memset(szKey, 0x20, sizeof(szKey));
					if (n==1) {
						if ((strcmp(pKey[i]->type, "PR") == 0) || (strcmp(pKey[i]->type, "FX") == 0) )
							sprintf(szKey, " KEY (%d,%d,P",  pKey[i]->pos, pKey[i]->len);
						else
							sprintf(szKey, " KEY (%d,%d,P",  pKey[i]->pos, pKey[i]->len);
						strcpy(szBuf, szKey);
					}
					else 
					{
						if ((strcmp(pKey[i]->type, "PR") == 0) || (strcmp(pKey[i]->type, "FX") == 0) )
							sprintf(szKey, ",%d,%d,AD",  pKey[i]->pos, pKey[i]->len);
						else
							sprintf(szKey, ",%d,%d,AD", pKey[i]->pos, pKey[i]->len);
						strcat(szBuf, szKey);
					}
					n++;
				}
			}
			i++;
			if (i > params->nMaxFields)
				i=0;
		} while (n <= params->nNumKeys);
        strcat(szBuf,")");
	}
    strcpy(pBuf, szBuf);
    return ;
}

int generate_Takefile ( struct params_t* params, struct key_t** pKey ) {

	FILE* pFile;
	char* pBuf;
    char* pBufTmp;
	int i,n;
	char szszTakeFile[FILENAME_MAX];

	char szKey[256];
	char szBuf[256];

	strcpy(szszTakeFile, params->PathTake);
	strcat(szszTakeFile, params->szFileName);
	strcat(szszTakeFile, ".prm");

	pBuf = (char*) malloc(MAX_RECORD+2);//(char*) malloc(params->nLenMax+2);
	memset(pBuf, 0x20, params->nLenMax+1);
	memset(szBuf, 0x20, sizeof(szBuf));

	pFile=fopen(szszTakeFile, "w");

	if (pFile==NULL){
		fprintf(stderr,"*  *S006* Cannot open file %s : %s\n",szszTakeFile,strerror(errno));
		return -1;
	}

	print_paramCFG( pFile, params, pKey);

	fprintf(stdout,"File parameters for GCSORT TAKE command\n");
	fprintf(stdout,"%s\n", szszTakeFile);
	fprintf(stdout,"------------------------------------------------------------------------------\n");

	if (strcmp(params->szRec, "F")==0) {
		sprintf((char*) pBuf, " USE  %s%s       ORG %s RECORD %s,%05d \n", params->PathGen, params->szFileName, params->szOrg, params->szRec, params->nLenMax);
	}
	else
	{
		sprintf((char*) pBuf, " USE  %s%s       ORG %s RECORD %s,%05d,%05d\n", params->PathGen, params->szFileName, params->szOrg, params->szRec, params->nLenMin, params->nLenMax);
	}

    if (strcmp(params->szOrg, "IX") == 0) {
        pBufTmp = (char*) malloc(256);
        SetKeyForSort(params, pKey, pBufTmp);
        strcat(pBuf, pBufTmp);
        free(pBufTmp);
    }

	fwrite(pBuf, sizeof(char), strlen(pBuf), pFile);

	if (strcmp(params->szRec, "F")==0) {
		sprintf((char*) pBuf, " GIVE %s%s.srt   ORG %s RECORD %s,%05d \n", params->PathGen, params->szFileName, params->szOrg, params->szRec, params->nLenMax);
	}
	else
	{
		sprintf((char*) pBuf, " GIVE %s%s.srt   ORG %s RECORD %s,%05d,%05d\n", params->PathGen, params->szFileName, params->szOrg, params->szRec, params->nLenMin, params->nLenMax);
	}

    if (strcmp(params->szOrg, "IX") == 0) {
        pBufTmp = (char*) malloc(256);
        SetKeyForSort(params, pKey, pBufTmp);
        strcat(pBuf, pBufTmp);
        free(pBufTmp);
    }


	fwrite(pBuf, sizeof(char), strlen(pBuf), pFile);


	if (params->nNumKeys > 0) {
		n=1;
		i=0;
		memset(szBuf, 0x20, sizeof(szBuf));
		do {
			if (pKey[i]->iskey == 1) {
				if (pKey[i]->seqkey == n) {
					memset(szKey, 0x20, sizeof(szKey));
					if (n==1) {
						if ((strcmp(pKey[i]->type, "PR") == 0) || (strcmp(pKey[i]->type, "FX") == 0) )
							sprintf(szKey, "%05d,%05d,CH,%s",  pKey[i]->pos, pKey[i]->len, pKey[i]->order);
						else
							sprintf(szKey, "%05d,%05d,%s,%s",  pKey[i]->pos, pKey[i]->len, pKey[i]->type, pKey[i]->order);
						strcpy(szBuf, szKey);
					}
					else 
					{
						if ((strcmp(pKey[i]->type, "PR") == 0) || (strcmp(pKey[i]->type, "FX") == 0) )
							sprintf(szKey, "%05d,%05d,CH,%s",  pKey[i]->pos, pKey[i]->len, pKey[i]->order);
						else
							sprintf(szKey, ",%05d,%05d,%s,%s", pKey[i]->pos, pKey[i]->len, pKey[i]->type, pKey[i]->order);
						strcat(szBuf, szKey);
					}
					n++;
				}
			}
			i++;
			if (i > params->nMaxFields)
				i=0;
		} while (n <= params->nNumKeys);
		sprintf((char*) pBuf, " SORT FIELDS=(%s)\n", szBuf);
		fwrite(pBuf, sizeof(char), strlen(pBuf), pFile);
	}

	for (i=0; i < n_gcsort_bottom; i++) {
		fwrite(szgcsort_bottom[i], sizeof(char), strlen(szgcsort_bottom[i]), pFile);
	}

	fclose(pFile);

	free(pBuf);

	return 0;
}
 cob_field* gen_cob_field_make (int type, int digits, int scale, int flags, int nLen, int nData)
{
	cob_field       *field_ret;
	cob_field_attr	*attrArea;
	attrArea = (cob_field_attr*) malloc(sizeof(cob_field_attr));
	attrArea->type   = type;
	attrArea->digits = digits;
	attrArea->scale  = scale;
	attrArea->flags  = flags;
	attrArea->pic    = NULL;
	field_ret = (cob_field*)malloc(sizeof(cob_field));
	field_ret->attr = attrArea;
	field_ret->data = NULL;
	if (nData == 0) {
		field_ret->data = (unsigned char*) malloc((sizeof(unsigned char)*nLen)+1);
		memset(field_ret->data, 0x00, nLen);
	}
	field_ret->size = nLen;
    gen_setAttrib(attrArea, type, nLen);
	return field_ret;
}
int genfile_SetInfoForFile( struct params_t* params, struct key_t** pKey, cob_file* stFile, int nMode) 
{
    int i,n,m;
	int	k=0;

    stFile->select_name = _strdup((const char *)"masterseqfile");
	stFile->assign = gen_cob_field_make( COB_TYPE_ALPHANUMERIC, strlen(params->PathGen)+strlen(params->szFileName)+1, 0, 0, strlen(params->PathGen)+strlen(params->szFileName)+1, ALLOCATE_DATA);
    stFile->record = gen_cob_field_make( COB_TYPE_ALPHANUMERIC, params->nLenMax, 0, 0, params->nLenMax, ALLOCATE_DATA);
    if (strcmp(params->szRec, "V") == 0) 
		stFile->variable_record = gen_cob_field_make( COB_TYPE_NUMERIC_DISPLAY, 5, 0, 0, 5, ALLOCATE_DATA);
	else
		stFile->variable_record = NULL;

    stFile->file_status = (unsigned char*)malloc(3);
	memset (stFile->file_status, 0x00, 3);
    if (strcmp(params->szRec, "V") == 0) {
        stFile->record_min = params->nLenMin;                         
        stFile->record_max = params->nLenMax;            
    }
    else
    {
        stFile->record_min = params->nLenMax;        // Min = Max
        stFile->record_max = params->nLenMax;            
    }
    stFile->nkeys = params->nNumKeys;
	stFile->keys = NULL;
	stFile->file = NULL;  
	stFile->fd = -1;  
	stFile->access_mode = COB_ACCESS_SEQUENTIAL;
	stFile->lock_mode = 0; // COB_LOCK_AUTOMATIC; // 	COB_FILE_EXCLUSIVE; //0;
	stFile->open_mode = COB_OPEN_CLOSED;
	stFile->flag_optional = 0;
	stFile->last_open_mode = 0;
	stFile->flag_operation = 0;
	stFile->flag_nonexistent = 0;
	stFile->flag_end_of_file = 0;           
	stFile->flag_begin_of_file = 0;           
	stFile->flag_first_read = 0;           
	stFile->flag_read_done = 0;           
	stFile->flag_select_features = 0;           
	stFile->flag_needs_nl = 0;           
	stFile->flag_needs_top = 0;           
	stFile->file_version = 1;
	// COB_ORG_SEQUENTIAL:
    if (strcmp(params->szOrg, "SQ") == 0) {
			stFile->organization = COB_ORG_SEQUENTIAL;
            opt = 0;
    }
	// COB_ORG_LINE_SEQUENTIAL:
    if (strcmp(params->szOrg, "LS") == 0) {
			stFile->organization = COB_ORG_LINE_SEQUENTIAL;
			opt = COB_WRITE_BEFORE | COB_WRITE_LINES | 1;
    }
    //	COB_ORG_RELATIVE:		
    if (strcmp(params->szOrg, "RL") == 0) {
         stFile->keys = (cob_file_key*)(malloc (sizeof (cob_file_key) * 1));
         stFile->keys[0].field = gen_cob_field_make( COB_TYPE_NUMERIC_DISPLAY, 5, 0, 0, 5, ALLOCATE_DATA);
         stFile->keys[0].flag = 0;
         stFile->keys[0].offset = 0;
         stFile->organization = COB_ORG_RELATIVE;
    }
   //  COB_ORG_INDEXED:
    if (strcmp(params->szOrg, "IX") == 0) {
        n=1;
        k=0;
        i=0;
        //-->>stFile->keys = (cob_file_key*)(malloc (sizeof (cob_file_key) * params->nNumKeys));
        stFile->keys = (cob_file_key*)(malloc (sizeof (cob_file_key) * params->nNumKeys));
    	for (m=0; m <= params->nMaxFields; m++) {
    	    do {
		        if (pKey[i]->iskey == 1) {
			        if (pKey[i]->seqkey == n) {
                        stFile->keys[k].field = gen_cob_field_make( gen_getFieldTypeLIBCOBFlags(pKey[i]->type), 
                                pKey[i]->len, 0, 0, pKey[i]->len, NOALLOCATE_DATA);
        				stFile->keys[k].field->data = stFile->record->data+pKey[i]->pos-1;
                        stFile->keys[k].field->size = pKey[i]->len;
				        stFile->keys[k].flag = 0;
				        //if (tKeys->type == KEY_IDX_ALTERNATIVE_DUP)
					    stFile->keys[k].flag = 0;		
                        if (n>1)
					        stFile->keys[k].flag = 1;		// forces "with duplicates" for all secondary keys 
				        stFile->keys[k].offset = pKey[i]->pos-1;
				        n++;
                        k++;
			        }
		        }
		        i++;
		        if (i > params->nMaxFields)
			        i=0;
	        } while (n <= params->nNumKeys);
        }
        stFile->access_mode = COB_ACCESS_DYNAMIC;  
		stFile->organization = COB_ORG_INDEXED;
	}
	return 0;
//
}

// From internal type get flags for create/setting cob_field
int gen_getFieldTypeLIBCOBFlags(const char *type) 
{
    if (strcmp(type, "CH") == 0) 
        return COB_TYPE_ALPHANUMERIC;
    if (strcmp(type, "ZD") == 0) 
        return COB_TYPE_NUMERIC_DISPLAY;
    if (strcmp(type, "BI") == 0) 
        return COB_TYPE_NUMERIC_BINARY;
    if (strcmp(type, "FI") == 0) 
        return COB_TYPE_NUMERIC_BINARY;
    if (strcmp(type, "PD") == 0) 
        return COB_TYPE_NUMERIC_PACKED;
    if (strcmp(type, "PR") == 0) 
        return COB_TYPE_NUMERIC_DISPLAY;
    if (strcmp(type, "FX") == 0) 
        return COB_TYPE_ALPHANUMERIC;
	return -1;
}



int generate_file ( struct params_t* params, struct key_t** pKey )
{
	unsigned char* pBuf;
    int k;
	int64_t	n64Var;
	long int i;
	unsigned long n;
	char szProg[20];
	char szPr[20];
	char szNum[32];
	char szFileOut[FILENAME_MAX];
	unsigned char szBufZD[33];
	char * pEnd;
    cob_field*      g_CH;      
    cob_field*      g_BI;      
    cob_field*      g_FI;      
    cob_field*      g_FL;      
    cob_field*      g_PD;      
    cob_field*      g_ZD;      

    cob_file* stFileDef = NULL;		// info for cob_open, cob_read, cob_write, cob_close

	pBuf = (unsigned char*) malloc(params->nLenMax+2);
	memset(pBuf, 0x20, params->nLenMax);

	strcpy(szFileOut, params->PathGen);
	strcat(szFileOut, params->szFileName);

    stFileDef = (cob_file*) malloc(sizeof(cob_file));
    genfile_SetInfoForFile(params, pKey, stFileDef, 0);
    strcpy((char*)stFileDef->assign->data, szFileOut);
	cob_open(stFileDef,  COB_OPEN_OUTPUT, 0, NULL);
	if (atol((char *)stFileDef->file_status) != 0) {
		fprintf(stderr,"** Cannot open file %s - File Status (%c%c)\n", szFileOut, stFileDef->file_status[0], stFileDef->file_status[1]);
        exit(99);
	}

    // create field LIBCOB
    // Allocate field for compare
	g_CH = gen_cob_field_create();
	g_BI = gen_cob_field_create();
	g_FI = gen_cob_field_create();
	g_FL = gen_cob_field_create();
	g_PD = gen_cob_field_create();
	g_ZD = gen_cob_field_create();


	for (n=0; n < params->nNumRec; n++) {
		for (i=0; i <= params->nMaxFields; i++) {
// Progessive
			if (strcmp(pKey[i]->type, "PR") == 0) {	// Progressivo
				sprintf(szPr, "%%0%dd", pKey[i]->len);
				sprintf(szProg, szPr, n+1);
				memcpy((unsigned char*)pBuf+pKey[i]->pos-1, szProg, pKey[i]->len);
				continue;
			}
// Fixed value
			if (strcmp(pKey[i]->type, "FX") == 0) {	// Fixed value
				memcpy((unsigned char*)pBuf+pKey[i]->pos-1, pKey[i]->value, pKey[i]->len);
				continue;
			}
// FIELD_TYPE_BINARY:
			if (strcmp(pKey[i]->type, "BI") == 0) {	// 
				for(k=0; k<pKey[i]->len;k++) 
					szNum[k] = gen_char(GEN_TYPE_NUMBER);
				n64Var = _strtoll((const char*)szNum, &pEnd, 10);
				gen_cob_field_set(g_BI, COB_TYPE_NUMERIC_BINARY, pKey[i]->len, 0, COB_FLAG_BINARY_SWAP, pKey[i]->len);
                if (g_BI->data == NULL)
   	                g_BI->data = (unsigned char*) (cob_field*)malloc( g_BI->size );
                cob_set_int(g_BI, (int)n64Var);  // int
                memcpy((unsigned char*) pBuf+pKey[i]->pos-1, g_BI->data, g_BI->size);
            }
		
			if (strcmp(pKey[i]->type, "FI") == 0) {	// Fixed value
				for(k=0; k<pKey[i]->len;k++) 
					szNum[k] = gen_char(GEN_TYPE_NUMBER);
				n64Var = _strtoll((const char*)szNum, &pEnd, 10);
				gen_cob_field_set(g_FI, COB_TYPE_NUMERIC_BINARY, pKey[i]->len, 0, COB_FLAG_HAVE_SIGN | COB_FLAG_BINARY_SWAP, pKey[i]->len/2);
                if (g_FI->data == NULL)
       	            g_FI->data = (unsigned char*) (cob_field*)malloc( g_FI->size );
                cob_set_int(g_FI, (int)n64Var);  // int
                memcpy((unsigned char*) pBuf+pKey[i]->pos-1, g_FI->data, g_FI->size);
            }
// FIELD_TYPE_FLOATING:
			if (strcmp(pKey[i]->type, "FL") == 0) {	// Fixed value
				for(k=0; k<pKey[i]->len;k++) 
					szNum[k] = gen_char(GEN_TYPE_NUMBER);
				n64Var = _strtoll((const char*)szNum, &pEnd, 10);
				gen_cob_field_set(g_FI, COB_TYPE_NUMERIC_BINARY, pKey[i]->len, 0, COB_FLAG_HAVE_SIGN | COB_FLAG_BINARY_SWAP, pKey[i]->len/2);
                if (g_FI->data == NULL)
       	            g_FI->data = (unsigned char*) (cob_field*)malloc( g_FI->size );
                cob_set_int(g_FI, (int)n64Var);  // int
                memcpy((unsigned char*) pBuf+pKey[i]->pos-1, g_FI->data, g_FI->size);
            }
// FIELD_TYPE_PACKED:
			if (strcmp(pKey[i]->type, "PD") == 0) {	// Fixed value
				memset(szNum, 0x00, sizeof(szNum));
				for(k=0; k<pKey[i]->len;k++) 
					szNum[k] = gen_char(GEN_TYPE_NUMBER);
				n64Var = _strtoll((const char*)szNum, &pEnd, 10);
				gen_cob_field_set(g_PD, COB_TYPE_NUMERIC_PACKED, pKey[i]->len, 0, COB_FLAG_HAVE_SIGN, pKey[i]->len/2+1);
                if (g_PD->data == NULL)
   	                g_PD->data = (unsigned char*) (cob_field*)malloc( g_PD->size );
                cob_set_int(g_PD, (int)n64Var);  // int
                memcpy((unsigned char*) pBuf+pKey[i]->pos-1, g_PD->data, g_PD->size);
            }

			if (strcmp(pKey[i]->type, "CH") == 0) {	// Fixed value
                gen_cob_field_set(g_CH, COB_TYPE_ALPHANUMERIC, pKey[i]->len, 0, 0, pKey[i]->len); 
				for(k=0; k<pKey[i]->len;k++) 
					pBuf[pKey[i]->pos+k-1] = gen_char(GEN_TYPE_CHARUPP);
			}

			if (strcmp(pKey[i]->type, "ZD") == 0)  {
				memset(szBufZD, 0x30, sizeof(szBufZD));
				for(k=0; k<pKey[i]->len+1;k++) {
					if (k == 0)
						szBufZD[k] = gen_char(GEN_TYPE_SIGN); 
					if (k > 0)
						szBufZD[k] = gen_char(GEN_TYPE_NUMBER);
				}
				if (szBufZD[0] == '-')
					szBufZD[pKey[i]->len] += 0x40; //Negative
				memcpy((unsigned char*) pBuf+pKey[i]->pos-1, szBufZD+1, pKey[i]->len);
			}
		}
        gen_set_area(stFileDef, pBuf, params->nLenMax);	
        cob_write (stFileDef, stFileDef->record, opt, NULL, 0);
	    if (atol((char *)stFileDef->file_status) != 0) {
		    fprintf(stderr,"** Cannot write file %s - File Status (%c%c)\n", szFileOut, stFileDef->file_status[0], stFileDef->file_status[1]);
            exit(99);
	    }
		if((n%100000==0) && (n>0))
			fprintf(stdout,"records written %ld\n", n);


	}

	 gen_cob_field_destroy(g_CH, ALLOCATE_DATA);
	 gen_cob_field_destroy(g_BI, ALLOCATE_DATA);
	 gen_cob_field_destroy(g_FI, ALLOCATE_DATA);
	 gen_cob_field_destroy(g_FL, ALLOCATE_DATA);
	 gen_cob_field_destroy(g_PD, ALLOCATE_DATA);
	 gen_cob_field_destroy(g_ZD, ALLOCATE_DATA);

     
	fprintf(stdout,"Total records written %ld\n", n);

	cob_close(stFileDef, NULL, COB_CLOSE_NORMAL, 0);

//-->>
    destroy_file(params, stFileDef);
//-->>

	free(pBuf);

	return 0;
}

void destroy_file (struct params_t* params, cob_file* stFile)
{
	unsigned int k;

    if (stFile == NULL)
        return;

    free((void*)stFile->select_name);  
	free(stFile->file_status);  
	    // cob_field
	gen_cob_field_destroy(stFile->assign, ALLOCATE_DATA);
	gen_cob_field_destroy(stFile->record, ALLOCATE_DATA);
    if (stFile->variable_record != NULL)
	    gen_cob_field_destroy(stFile->variable_record, ALLOCATE_DATA);
    if (stFile->organization == COB_ORG_RELATIVE)  // delete field for relative 
    	    gen_cob_field_destroy(stFile->keys[0].field, ALLOCATE_DATA);
    if (stFile->organization == COB_ORG_INDEXED) { // delete field for indexed
        for (k=0; k<stFile->nkeys;k++) {
             if (stFile->keys != NULL)
		        gen_cob_field_destroy(stFile->keys[k].field, NOALLOCATE_DATA);
	    }
    }
	if (stFile->keys != NULL)
		free(stFile->keys); 
	free(stFile); 
//
}


int writeHeader (FILE* pFile, struct params_t* params, struct key_t** pKey )
{
	int i,n;
	fprintf(pFile,"* ----------------------------------------------------------------------------\n");
	fprintf(pFile,"* File generated from gcsort_gentestcase\n");
	fprintf(pFile,"* ----------------------------------------------------------------------------\n");
	fprintf(pFile,"* pathgen     =%s\n",	params->PathGen);
	fprintf(pFile,"* filename    =%s\n",	params->szFileName);
	fprintf(pFile,"* organization=%s\n",	params->szOrg);
	fprintf(pFile,"* record      =%s\n",	params->szRec);
	fprintf(pFile,"* lenmin      =%05d\n",	params->nLenMin);
	fprintf(pFile,"* lenmax      =%05d\n",	params->nLenMax);
	fprintf(pFile,"* numrec      =%05lu\n",	params->nNumRec);
	fprintf(pFile,"* ----------------------------------------------------------------------------\n");
	fprintf(pFile,"*                             Record definition\n");
	for (i=0; i <= params->nMaxFields; i++) {
		fprintf(pFile, (char*)  "* Field : %05d - Pos=%05d, Len=%05d, Type=%s", i+1, pKey[i]->pos, pKey[i]->len, pKey[i]->type);
		if ( pKey[i]->iskey == 1)
			fprintf(pFile, (char*)  ", Order=%s, KeySequence=%d\n", pKey[i]->order, pKey[i]->seqkey);
		else
			fprintf(pFile, "\n");
	}
	fprintf(pFile,"* ----------------------------------------------------------------------------\n");
	fprintf(pFile, "*                             Key definition\n");

	if (params->nNumKeys > 0) {
		n=1;
		i=0;
		do {
			if (pKey[i]->iskey == 1) {
				if (pKey[i]->seqkey == n) {
					fprintf(pFile, "* Key   : Order field %05d - KeySequence=%d - Pos=%05d, Len=%05d, Type=%s\n", i+1, pKey[i]->seqkey, pKey[i]->pos, pKey[i]->len, pKey[i]->type);
					n++;
				}
			}
			i++;
			if (i > params->nMaxFields)
				i=0;
		} while (n <= params->nNumKeys);
	}
	fprintf(pFile, "* ----------------------------------------------------------------------------\n");
	return 0;
}


char gen_char ( int nT ) 
{	
	char randomletter;
//

	switch(nT) {
	case GEN_TYPE_ALL:			// all char
		randomletter = szAllChar[rand () % 82];
		break;
	case GEN_TYPE_CHARUPP:			// Upper case letters
		randomletter = szCharUpp[rand () % 26];
		break;
	case GEN_TYPE_CHARLOW:			// lower case letters
		randomletter = szCharLow[rand () % 26];
		break;
	case GEN_TYPE_CHARALL:			// upper and lowercase
		randomletter = szChar[rand () % 52];
			break;
	case GEN_TYPE_NUMBER:			// number
		randomletter = szNum[rand () % 10];
		break;
	case GEN_TYPE_SIGN:			// number
		randomletter = szSign[rand () % 2];
		break;
	}
	return randomletter;
}

void write_line (FILE* pFile, char* pBuf)
{
	fwrite(pBuf, sizeof(char), strlen(pBuf), pFile);
}
void CreateExampleFileConfig (void ) 
{
	FILE* pFile;

	fprintf(stdout, "Generating .... file example_gen.cfg .. with default values...\n");

	pFile=fopen("example_gen.cfg", "w");
	if (pFile==NULL){
		fprintf(stderr,"*  *S005* Cannot open file example_gen.cfg : %s\n", strerror(errno));
		return ;
	}

	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"*\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*  Config file for file generator\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*\n");
	write_line(pFile, (char*)"*\n");
	write_line(pFile, (char*)"*(0)  Section\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"* name and pathname\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"script=execmasterscript\n");
	write_line(pFile, (char*)"pgmcheckdata=TSTCHDATAX\n");
	write_line(pFile, (char*)"pgmchecksort=TSTCHSORTX\n");
	write_line(pFile, (char*)"pathgen=../files\n");
	write_line(pFile, (char*)"pathsrc=../src\n");
	write_line(pFile, (char*)"pathtake=../take\n");
	write_line(pFile, (char*)"pathbatsh=../script\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*  Definitions for environment variables script\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"scripthead_start\n");
	write_line(pFile, (char*)"export LD_LIBRARY_PATH=/usr/local/lib\n");
	write_line(pFile, (char*)"scripthead_end\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*  Definitions for parameters for behaviour GCSORT\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"gcsort_top_start\n");
	write_line(pFile, (char*)"GCSORT_PATHTMP=./\n");
	write_line(pFile, (char*)"GCSORT_MEMSIZE=512000000\n");
	write_line(pFile, (char*)"GCSORT_DEBUG=0\n");
	write_line(pFile, (char*)"GCSORT_STATISTICS=2\n");
	write_line(pFile, (char*)"GCSORT_SLOT=1\n");
	write_line(pFile, (char*)"GCSORT_MLT=63\n");
	write_line(pFile, (char*)"gcsort_top_end\n");
	write_line(pFile, (char*)"\n");

	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*  Definitions for Take files \n");
	write_line(pFile, (char*)"*  more command after SORT command Use, Give\n");
	write_line(pFile, (char*)"*  You can insert command Outrec, Inrec, OutFil\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"gcsort_bottom_start\n");
	write_line(pFile, (char*)"** Use  with caution because Cobol program, that checks sorted data, \n");
	write_line(pFile, (char*)"** can fail if OUTREC/INREC changes the structure of the keys.\n");
	write_line(pFile, (char*)"**\n");
	write_line(pFile, (char*)"* \n");
	write_line(pFile, (char*)"** Example with INCLUDE. \n");
	write_line(pFile, (char*)"** Filter the first 50 records from input  \n");
	write_line(pFile, (char*)"** checking value of progressive record number.\n");
	write_line(pFile, (char*)"*\n");
	write_line(pFile, (char*)"** Example INCLUDE COND=(1,5,CH,LE,C'00050')\n");
	write_line(pFile, (char*)"gcsort_bottom_end\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"\n");

	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*(1) File Name\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"filename=filegen.sq\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*(2) Organization  LS = line sequential, SQ = sequential\n");
	write_line(pFile, (char*)"*                  RL = relative       , IX = indexed   \n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
//	write_line(pFile, (char*)"organization=SQ\n");
//	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*(3) Record  F = fixed, V = variable                  \n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"* record=V\n");
	write_line(pFile, (char*)"record=F\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*(4) Len min, Len max    for Fixed lenmin = lenmax, for Variable lenmax without 4 byte record len\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"lenmin=30\n");
	write_line(pFile, (char*)"lenmax=100\n");
	write_line(pFile, (char*)"#lenmax=994\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"* byteorder 0 = native, 1=Bigendian\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"byteorder=0     \n");
	write_line(pFile, (char*)"   \n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*(5) Records number\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"**\n");
	write_line(pFile, (char*)"** numrec=5500000      \n");
	write_line(pFile, (char*)"** \n");
	write_line(pFile, (char*)"numrec=100\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*(6) Key   three elements for field \n");
	write_line(pFile, (char*)"* ----------------------------------------------------\n");
	write_line(pFile, (char*)"*     Pos, Len, Type\n");
	write_line(pFile, (char*)"pos=1\n");
	write_line(pFile, (char*)"len=5\n");
	write_line(pFile, (char*)"type=PR\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=6\n");
	write_line(pFile, (char*)"len=5\n");
	write_line(pFile, (char*)"type=PD\n");
	write_line(pFile, (char*)"iskey=yes\n");
	write_line(pFile, (char*)"seqkey=3\n");
	write_line(pFile, (char*)"order=A\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=11\n");
	write_line(pFile, (char*)"len=9\n");
	write_line(pFile, (char*)"type=CH\n");
	write_line(pFile, (char*)"iskey=yes\n");
	write_line(pFile, (char*)"seqkey=1\n");
	write_line(pFile, (char*)"order=A\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=20\n");
	write_line(pFile, (char*)"len=3\n");
	write_line(pFile, (char*)"type=BI\n");
	write_line(pFile, (char*)"iskey=yes\n");
	write_line(pFile, (char*)"seqkey=2\n");
	write_line(pFile, (char*)"order=A\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=23\n");
	write_line(pFile, (char*)"len=3\n");
	write_line(pFile, (char*)"type=FI\n");
	write_line(pFile, (char*)"iskey=yes\n");
	write_line(pFile, (char*)"seqkey=4\n");
	write_line(pFile, (char*)"order=A\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=26\n");
	write_line(pFile, (char*)"len=10\n");
	write_line(pFile, (char*)"type=FX\n");
	write_line(pFile, (char*)"value=AAAAAAAAAA\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=36\n");
	write_line(pFile, (char*)"len=10\n");
	write_line(pFile, (char*)"type=CH\n");
	write_line(pFile, (char*)"iskey=yes\n");
	write_line(pFile, (char*)"seqkey=6\n");
	write_line(pFile, (char*)"order=D\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=46\n");
	write_line(pFile, (char*)"len=3\n");
	write_line(pFile, (char*)"type=FX\n");
	write_line(pFile, (char*)"value=ZZZ\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=49\n");
	write_line(pFile, (char*)"len=5\n");
	write_line(pFile, (char*)"type=CH\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"pos=54\n");
	write_line(pFile, (char*)"len=12\n");
	write_line(pFile, (char*)"type=ZD\n");
	write_line(pFile, (char*)"iskey=yes\n");
	write_line(pFile, (char*)"seqkey=5\n");
	write_line(pFile, (char*)"order=A\n");
	write_line(pFile, (char*)"\n");
	write_line(pFile, (char*)"* ...\n");
	write_line(pFile, (char*)"* ...\n");
	write_line(pFile, (char*)"* ..\n");
	write_line(pFile, (char*)"\n");
	
	fclose(pFile);

	fprintf(stdout, "Generated.\n");
	fprintf(stdout, "Edit the values for folders, the environment variables,... depending on your configuration.\n");
	return;
}
