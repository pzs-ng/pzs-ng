#include <sys/types.h>
#include <regex.h>

#include "handling.h"

#include "zsfunctions.h"

/* returns -1 on fail, so NO FILE TYPE CAN HAVE NUMBER -! */
short
get_filetype(char *fileext)
{

	/* let's hope no system defines a regcomp error as -1 */
	int status = -1;

	regex_t    re;
	FEXT *fext;

	for (fext = fexts; fext->ext != 0; fext++) {

		if (status == 0)
			regfree(&re);
		
		if ((status = regcomp(&re, fext->ext, REG_EXTENDED | REG_NOSUB) != 0)) {
			d_log(1, "get_filetype(%s): regcomp failed: %s", fileext, strerror(errno));
			return -1;
		}
		
		if (regexec(&re, fileext, (size_t) 0, NULL, 0) == 0) {
			/* type is defined for file extension */
			regfree(&re);
			return fext->type;
		}

	}

	regfree(&re);

	/* fileext isn't a defined type,
	 * a default handler will be used */
	return 0;

}

handler_t *
get_handler(short type)
{
	
	HANDLER *hand;
	
	for (hand = handlers; hand->handler != 0; hand++) {
		if (type == hand->type) {
			/* handler exists for type */
			return hand->handler;
		}
	}

	/* no handler for specified type
	 * should not reach this */
	return 0;

}

