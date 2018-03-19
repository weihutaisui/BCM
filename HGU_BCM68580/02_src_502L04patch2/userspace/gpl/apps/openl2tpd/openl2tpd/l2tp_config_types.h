#ifndef _L2TP_CONFIG_TYPES_H
#define _L2TP_CONFIG_TYPES_H

/* This typedef is used by the lex/yacc parser to pass strings or
 * blobs of binary data from the lexical analyzer to the syntax
 * parser.
 */
typedef struct {
	unsigned int len;
	unsigned char *buf;
} l2tp_byte_vector_t;

#endif /* _L2TP_CONFIG_TYPES_H */
