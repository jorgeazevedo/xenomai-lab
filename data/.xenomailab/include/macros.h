/*
 * Xenomai Lab
 * Copyright (C) 2013 Jorge Azevedo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//Commenting this define disables debug messages
#define DEBUGON

#define WARNING(...) fprintf(stderr, "W: %17s: ", __PRETTY_FUNCTION__),fprintf(stderr,__VA_ARGS__),fflush(stderr)

#define ERROR(...) fprintf(stderr, "E: %17s: ", __PRETTY_FUNCTION__),fprintf(stderr,__VA_ARGS__),fflush(stderr),exit(1)

#define RETERROR(...) {fprintf(stderr, "E: %17s: ", __PRETTY_FUNCTION__);fprintf(stderr,__VA_ARGS__);fflush(stderr);return 1;}

#ifdef DEBUGON

        #define DEBUG(...) printf("%20s: \t", __PRETTY_FUNCTION__),printf(__VA_ARGS__)
#else
        #define	DEBUG(...)
#endif
