#ifndef COLUMN_H_INCLUDED
#define COLUMN_H_INCLUDED

enum column_type {
    column_type_small_integer,  
    column_type_integer,  
    column_type_big_integer,  
    column_type_rational,
    column_type_big_rational,
    column_type_boolean,
    column_type_char
};

struct column_metadata {
    enum column_type type_code;
    unsigned int parameter;
};

#endif /* COLUMN_H_INCLUDED */
