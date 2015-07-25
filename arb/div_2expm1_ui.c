/*=============================================================================

    This file is part of ARB.

    ARB is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ARB is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ARB; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2015 Fredrik Johansson

******************************************************************************/

#include "arb.h"

void
arb_div_2expm1_ui(arb_t y, const arb_t x, ulong n, long prec)
{
    if (n < FLINT_BITS)
    {
        arb_div_ui(y, x, (1UL << n) - 1, prec);
    }
    else if (n < 1024 + prec / 32 || n > LONG_MAX / 4)
    {
        arb_t t;
        fmpz_t e;

        arb_init(t);
        fmpz_init_set_ui(e, n);

        arb_one(t);
        arb_mul_2exp_fmpz(t, t, e);
        arb_sub_ui(t, t, 1, prec);
        arb_div(y, x, t, prec);

        arb_clear(t);
        fmpz_clear(e);
    }
    else
    {
        arb_t s, t;
        long i, b;

        arb_init(s);
        arb_init(t);

        /* x / (2^n - 1) = sum_{k>=1} x * 2^(-k*n)*/
        arb_mul_2exp_si(s, x, -n);
        arb_set(t, s);
        b = 1;

        for (i = 2; i <= prec / n + 1; i++)
        {
            arb_mul_2exp_si(t, t, -n);
            arb_add(s, s, t, prec);
            b = i;
        }

        /* error bound: sum_{k>b} x * 2^(-k*n) <= x * 2^(-b*n - (n-1)) */
        arb_mul_2exp_si(t, x, -b*n - (n-1));
        arb_abs(t, t);
        arb_add_error(s, t);

        arb_set(y, s);

        arb_clear(s);
        arb_clear(t);
    }
}
