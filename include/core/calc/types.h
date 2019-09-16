/*
 * types.h
 *
 *  Created on: 16 сент. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_CALC_TYPES_H_
#define CORE_CALC_TYPES_H_

#include <common/types.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace calc
    {
        enum value_type_t
        {
            VT_NULL,
            VT_UNDEF,
            VT_INT,
            VT_FLOAT,
            VT_STRING,
            VT_BOOL
        };

        typedef struct value_t
        {
            value_type_t    type;
            union
            {
                ssize_t         v_int;
                double          v_float;
                LSPString      *v_str;
                bool            v_bool;
            };
        } value_t;
    }
}

#endif /* CORE_CALC_TYPES_H_ */
