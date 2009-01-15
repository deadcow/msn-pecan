/**
 * Copyright (C) 2008 Devid Antonio Filoni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <string.h>
#include <stdlib.h>

char* remove_plus_tags_from_str (const char* str)
{
	char *next_code, *final_str = NULL;

	if (str)
		final_str = strdup (str);
	else
		return NULL;

	int code_number;
	char *code[30];
	int codelen[30];

	code[0] = "[c=";

	code[1] = "[/c";

	code[2] = "[b]";
	codelen[2] = 3;

	code[3] = "[/b]";
	codelen[3] = 4;

	code[4] = "[i]";
	codelen[4] = 3;

	code[5] = "[/i]";
	codelen[5] = 4;

	code[6] = "[u]";
	codelen[6] = 3;

	code[7] = "[/u]";
	codelen[7] = 4;

	code[8] = "[s]";
	codelen[8] = 3;

	code[9] = "[/s]";
	codelen[9] = 4;

	code[10] = "[a=";

	code[11] = "[/a";

	code[12] = "·$";

	code[13] = "·#";
	codelen[13] = 3;

	code[14] = "·&";
	codelen[14] = 3;

	code[15] = "·@";
	codelen[15] = 3;

	code[16] = "·'";
	codelen[16] = 3;

	code[17] = "·0";
	codelen[17] = 3;

	code[18] = "&#x5B;c&#x3D;";

	code[19] = "&#x5B;/c";

	code[20] = "&#x5B;b&#x5D;";
	codelen[20] = 13;

	code[21] = "&#x5B;/b&#x5D;";
	codelen[21] = 14;

	code[22] = "&#x5B;i&#x5D;";
	codelen[22] = 13;

	code[23] = "&#x5B;/i&#x5D;";
	codelen[23] = 14;

	code[24] = "&#x5B;u&#x5D;";
	codelen[24] = 13;

	code[25] = "&#x5B;/u&#x5D;";
	codelen[25] = 14;

	code[26] = "&#x5B;s&#x5D;";
	codelen[26] = 13;

	code[27] = "&#x5B;/s&#x5D;";
	codelen[27] = 14;

	code[28] = "&#x5B;a&#x3D;";

	code[29] = "&#x5B;/a";

	for (code_number = 0; code_number < 30; code_number++)
	{
		int occurences = 0;
		char *parsed_str = NULL;

		if (final_str)
			parsed_str = calloc (strlen (final_str), strlen (final_str));
		if (!parsed_str)
			return NULL;

		next_code = strstr (final_str, code[code_number]);
		while (next_code)
		{
			if (code_number == 0 || code_number == 1 || code_number == 10 || code_number == 11)
			{
				if (strstr (next_code, "]"))
					codelen[code_number] = strlen (next_code)-strlen (strstr (next_code, "]"))+1;
				else
					codelen[code_number] = 0;
			}
			else if (code_number == 12)
			{
				if (strlen (next_code) == 3)
					codelen[12] = 3;
				else if (strncmp (next_code+3, "#", 1) == 0)
					codelen[12] = 10;
				else
				{
					if (strstr (next_code, ","))
					{
						if (strlen (next_code)-4 == strlen (strstr (next_code, ",")))
							codelen[12] = 6;
						else if (strlen (next_code)-5 == strlen (strstr (next_code, ",")) &&
							 strncmp (next_code+3, "1", 1) == 0 && (strncmp (next_code+4, "0", 1) == 0 ||
							 strncmp (next_code+4, "1", 1) == 0 || strncmp (next_code+4, "2", 1) == 0 ||
							 strncmp (next_code+4, "3", 1) == 0 || strncmp (next_code+4, "4", 1) == 0 ||
							 strncmp (next_code+4, "5", 1) == 0))
							codelen[12] = 7;
						else
							codelen[12] = 4;
					}
					else
						codelen[12] = 4;

					if (strncmp (next_code+codelen[code_number]-1, "1", 1) == 0 &&
					    (strncmp (next_code+codelen[code_number], "0", 1) == 0 ||
					     strncmp (next_code+codelen[code_number], "1", 1) == 0 ||
					     strncmp (next_code+codelen[code_number], "2", 1) == 0 ||
					     strncmp (next_code+codelen[code_number], "3", 1) == 0 ||
					     strncmp (next_code+codelen[code_number], "4", 1) == 0 ||
					     strncmp (next_code+codelen[code_number], "5", 1) == 0))
						codelen[12]++;
				 }
			}
			else if (code_number == 18 || code_number == 19 || code_number == 28 || code_number == 29)
			{
				if (strstr (next_code, "&#x5D;"))
					codelen[code_number] = strlen (next_code)-strlen (strstr (next_code, "&#x5D;"))+6;
				else
					codelen[code_number] = 0;
			}

			if (codelen[code_number] != 0)
			{
				strncat (parsed_str, final_str+occurences+strlen (parsed_str),
					 strlen (final_str)-strlen (parsed_str)-strlen (next_code)-occurences);
				occurences += codelen[code_number];

				if (next_code+codelen[code_number])
					next_code = strstr (next_code+codelen[code_number], code[code_number]);
				else
					next_code = NULL;
			}
			else
				if (next_code+1)
					next_code = strstr (next_code+1, code[code_number]);
				else
					next_code = NULL;
		}
		strcat (parsed_str, final_str+occurences+strlen (parsed_str));

		memcpy (final_str, parsed_str, strlen (final_str));
		free (parsed_str);
	}

	return final_str;
}
