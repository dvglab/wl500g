/*
 *  Template MIB group implementation - example.c
 *
 */

/* include important headers */
#include <config.h>

/* needed by util_funcs.h */
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

/* mibincl.h contains all the snmp specific headers to define the
   return types and various defines and structures. */
#include "mibincl.h"

/* header_generic() comes from here */
#include "util_funcs.h"

/* include our .h file */
#include "example.h"

	/*********************
	 *
	 *  Initialisation & common implementation functions
	 *
	 *********************/


/* this is an optional function called at the time the agent starts up
   to do any initilizations you might require.  You don't have to
   create it, as it is optional. */

/* IMPORTANT: If you add or remove this function, you *must* re-run
   the configure script as it checks for its existance. */

void init_example(void)
{
/* Define a 'variable' structure that is a representation of our mib. */

/* first, we have to pick the variable type.  They are all defined in
   the var_struct.h file in the agent subdirectory.  I'm picking the
   variable2 structure since the longest sub-component of the oid I
   want to load is .2.1 and .2.2 so I need at most 2 spaces in the
   last entry. */

  struct variable2 example_variables[] = {
    { EXAMPLESTRING,  ASN_OCTET_STR, RONLY, var_example, 1, {1}},
    /* Load the first table entry.  arguments:
       1: EXAMPLESTRING: magic number to pass back to us and used to
       check against the incoming mib oid requested
       later.
       2: STRING: type of value returned.
       3: RONLY: Its read-only.  We can't use 'snmpset' to set it.
       4: var_example: The return callback function, defined in example.c
       5: 1: The length of the next miboid this will be located at

       6: {1}: The sub-oid this table entry is for.  It's appended to
       the table entry defined later.
    */

    /* Now do the rest.  The next two are the sub-table, at .2.1 and .2.2: */
    { EXAMPLEINTEGER,   ASN_INTEGER,   RONLY, var_example, 2, {2,1}},
    { EXAMPLEOBJECTID,  ASN_OBJECT_ID, RONLY, var_example, 2, {2,2}},
    { EXAMPLETIMETICKS, ASN_TIMETICKS, RONLY, var_example, 1, {3}},
    { EXAMPLEIPADDRESS, ASN_IPADDRESS, RONLY, var_example, 1, {4}},
    { EXAMPLECOUNTER,   ASN_COUNTER,   RONLY, var_example, 1, {5}},
    { EXAMPLEGAUGE,     ASN_GAUGE,     RONLY, var_example, 1, {6}}
  };

  /* Define the OID pointer to the top of the mib tree that we're
   registering underneath. */
  oid example_variables_oid[] = { 1,3,6,1,4,1,2021,254 };

  /* register ourselves with the agent to handle our mib tree

   This is a macro defined in ../../snmp_vars.h.  The arguments are:

   descr:   A short description of the mib group being loaded.
   var:     The variable structure to load.
   vartype: The variable structure used to define it (variable2, variable4, ...)
   theoid:  A *initialized* *exact length* oid pointer.
            (sizeof(theoid) *must* return the number of elements!)  
  */
  REGISTER_MIB("example", example_variables, variable2, example_variables_oid);

  /* call auto_nlist to load the nlist symbols.  We
     actually don't need it, so its commented out. */
  /* auto_nlist( "example_symbol" ); */
}

#if USING_DLMOD_MODULE

static struct subtree example_subtree = {
	{1,3,6,1,4,1,2021,254}, 8, example_variables, 
	sizeof(example_variables)/sizeof(*example_variables), 
	sizeof(*example_variables), "example"
};

int example_init(void) {
  load_subtree(&example_subtree);
  return 0;
}
#endif
	/*********************
	 *
	 *  System specific implementation functions
	 *
	 *********************/

/* define the callback function used in the example_variables
   structure in example.h */

u_char	*
var_example(struct variable *vp,
	    oid *name,
	    int *length,
	    int exact,
	    int *var_len,
	    WriteMethod **write_method)
{
  /* define any variables we might return as static! */
  static long long_ret;
  static char string[300];
  static oid oid_ret[8];
  
  /* header_generic is a simple function for finding out if we're in
     the right place.  This only works on scalar objects.  Use
     header_simple_table for simple tables, and write your own for anything
     else. */
  if (header_generic(vp, name, length, exact, var_len, write_method))
    return NULL;

  /* We can now simply test on vp's magic number, defined in example.h */
  switch (vp->magic){
    case EXAMPLESTRING:
      /* set up return information */
      sprintf(string,"life the universe and everything");
      
      /* set the length of the returned data */
      *var_len = strlen(string);
      
      /* return everything as mapped to a u_char * */
      return (u_char *) string;

    case EXAMPLEINTEGER:
      long_ret = 42;
      return (u_char *) &long_ret;
      /* note: var_len defaults to the length of a long */
      
    case EXAMPLEOBJECTID:
      oid_ret[0] = 1;
      oid_ret[1] = 3;
      oid_ret[2] = 6;
      oid_ret[3] = 1;
      oid_ret[4] = 4;
      oid_ret[5] = oid_ret[6] = oid_ret[7] = 42;
      *var_len = 8*sizeof(oid);
      return (u_char *) oid_ret;
      
    case EXAMPLETIMETICKS:
      long_ret = 363136200;
      return (u_char *) &long_ret;
      
    case EXAMPLEIPADDRESS:
      /* ipaddresses get returned as a long.  ick */
      /* we're returning 127.0.0.1 */
      long_ret = ((127 << (8*3)) + (0 << (8*2)) + (0 << (8*1)) + 1);
      return (u_char *) &long_ret;
      
    case EXAMPLECOUNTER:
      long_ret = 42;
      return (u_char *) &long_ret;
      
    case EXAMPLEGAUGE:
      long_ret = 42;
      return (u_char *) &long_ret;

    default:
      ERROR_MSG("example.c: don't know how to handle this request.");
  }
  /* if we fall to here, fail by returning NULL */
  return NULL;
}
