#ifndef _FSM_PARSER_H_
#define _FSM_PARSER_H_

#include "ss_data.h"

/*********************************************
 * Definition of buffer size allocation per HP 
 *********************************************/

/*************************************
 * x: interval count within the HP
 * y: job count within the HP
 *************************************/

/*
 * we are using carriage return and eol '\r\n' to avoid conflict
 */
#define EOL_LENGTH (2)


/*
 * HP field calculation
 */

/* 
 * field length + sizeof(',') 
 */
#define HP_PER_FIELD (MAX_NON_STRING_FIELD_LENGTH + 1)

/* -1 is to compenseate , */
#define HP_FIELD_LENGTH (HP_PER_FIELD * HP_FIELD_COUNT + (EOL_LENGTH - 1) )

/*intr:est, association 
 * 2 == sizeof delimiters, i.e. ','  &&  ':'
 * MAX_NON_STRING_FIELD_LENGTH * 2 == (intr and est)
 * TODO: make multiplying 2 configurable
*/
#define JOB_FIELD_PER_INTR_BOND_LENGTH ((MAX_NON_STRING_FIELD_LENGTH*2) + 2)

#define JOB_INTR_BOND_LEN(count) (count * JOB_FIELD_PER_INTR_BOND_LENGTH)  
/*
* interval length calculation
*/
/* INTERVAL_FIELD_COUNT : number of parameters in the field
 * MAX_NON_STRING_FILED_LENGTH: the constant field width
 * INTERVAL_FILED_COUNT+1 : count of delimiters (includes EOL where EOL == '\r\n')
 */
#define PER_INTERVAL_LENGTH ((INTERVAL_FIELD_COUNT * MAX_NON_STRING_FIELD_LENGTH) + (INTERVAL_FIELD_COUNT + 1) )

#define INTERVAL_LENGTH(x)  ((x* PER_INTERVAL_LENGTH) + (DATA_SEPERATOR_COUNT(2)) )

/*
* job length calculation
*/

#define PER_JOB_INTR_DELIMITER_LENGTH (2)
/*
 * JOB_FIELD_COUNT-2: total field no excluding non string and interval bond params 
 * MAX_NON_STRING_FIELD_LENGTH: length of per field count
 * JOB_FIELD_COUNT+1: total delimiters includes EOL
 * PER_JOB_INTR_DELIMITER_LENGTH: includes '[ ]' which is 2
 * PATHMAX:  path length.
 * note: does not include intr bond length as it is dynamic which is handled in HP header.
*/
#define PER_JOB_LENGTH ((((JOB_FIELD_COUNT-2) * MAX_NON_STRING_FIELD_LENGTH)+ (JOB_FIELD_COUNT+1)) + \
			PER_JOB_INTR_DELIMITER_LENGTH + PATH_MAX )

/*
 *	x: total number of jobs
*	y: total number of interval job bond.
*/
#define JOB_LENGTH(x, y)  (((x* PER_JOB_LENGTH) + JOB_INTR_BOND_LEN(y)) + (DATA_SEPERATOR_COUNT(2)) )  

/*
 * delimiters count + data size
 * DS : Data Seperator
*/
#define DS_MAGIC_NO_LENGTH (4)

/* 
 * magic number length + delimiter length + EOL or '\r\n' length
 */
/* 
 * sizeof('!'+ '\r\n') == 3 
 */
#define DS_DELIMITER_LENGTH (3) 
#define DS_PER_LENGTH (DS_MAGIC_NO_LENGTH + DS_DELIMITER_LENGTH)


/* NO_OF_DATA_SEPERATORS : because we read the header
 * with first data seperator to calculate rest,
 * This is used for mallocing the buffer of per HP.
 */
#define DATA_SEPERATOR_COUNT(x) (x * DS_PER_LENGTH)

/*
* buffer length of a HP
* x: interval count
* y: job count
* z: intr job bond count.
*/
#define BUFFER_LENGTH(x,y,z) ( INTERVAL_LENGTH(x)+ JOB_LENGTH(y,z) + DATA_SEPERATOR_COUNT((NO_HP_DATA_SEPERATORS-1)))

/*
* buffer length of a aperiodic HP
* x: job count
* y: intr job
*/
#define BUFFER_LENGTH_APER(x,y) ( JOB_LENGTH(x,y) + DATA_SEPERATOR_COUNT((NO_HP_DATA_SEPERATORS - 1)))

#endif /* FSM_PARSER_H */

