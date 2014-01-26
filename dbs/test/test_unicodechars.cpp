/*
 * test_unicodechars.cpp
 *
 *  Created on: Jan 12, 2014
 *      Author: ipopa
 */

#include <assert.h>
#include <iostream>
#include <bitset>

#include "utils/wunicode.h"
#include "custom/include/test/test_fmw.h"
#include "dbs/dbs_mgr.h"
#include "dbs/dbs_values.h"

using namespace whisper;

static std::bitset<0x110000> sVerifiedChars;


static bool
test_character (const uint32_t codePoint,
                const uint32_t codePointUppercase,
                const uint32_t codePointLowercase,
                const uint32_t codePointCannonical)
{
  if (codePointLowercase == codePointUppercase)
    {
      if ((codePoint != codePointLowercase)
          || (codePoint != codePointCannonical)
          || (codePoint != wh_to_base_letter (codePoint))
          || (wh_to_lowercase (codePoint) != codePointLowercase)
          || (wh_to_uppercase (codePoint) != codePointUppercase))
        {
          return false;
        }
    }

  else if (codePoint == codePointUppercase)
    {
      if (! wh_is_uppercase (codePoint)
          || wh_is_lowercase (codePoint)
          || (wh_to_base_letter (codePoint) != codePointCannonical)
          || (wh_to_lowercase (codePoint) != codePointLowercase)
          || (wh_to_uppercase (codePoint) != codePointUppercase))
        {
          return false;
        }
    }
  else if (codePoint == codePointLowercase)
    {
      if (wh_is_uppercase (codePoint)
          || ! wh_is_lowercase (codePoint)
          || (wh_to_base_letter (codePoint) != codePointCannonical)
          || (wh_to_lowercase (codePoint) != codePointLowercase)
          || (wh_to_uppercase (codePoint) != codePointUppercase))
        {
          return false;
        }
    }
  else
    return false;

  sVerifiedChars.set (codePoint);

  return true;
}


static bool
test_latin_base_char_set ()
{
  bool result = true;

  std::cout << "Testing Latin base character set...";

  for (uint32_t i = 0; (i < 65) && result; ++i)
    result = test_character (i, i, i, i);

  for (uint32_t i = 65; (i < 91) && result; ++i)
    result = test_character (i, i, i + 32, i);

  for (uint32_t i = 91; (i < 97) && result; ++i)
    result = test_character (i, i, i, i);

  for (uint32_t i = 97; (i < 123) && result; ++i)
    result = test_character (i, i - 32, i, i);

  for (uint32_t i = 123; (i < 128) && result; ++i)
    result = test_character (i, i, i, i);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}


static bool
test_latin_1_supplement_char_set ()
{
  bool result = true;

  std::cout << "Testing Latin 1 supplement character set...";

  for (uint32_t i = 128; (i < 192) && result; ++i)
    result = test_character (i, i, i, i);



  result &= test_character (192, 192, 224, 65);
  result &= test_character (224, 192, 224, 97);


  result &= test_character (193, 193, 225, 65);
  result &= test_character (225, 193, 225, 97);


  result &= test_character (194, 194, 226, 65);
  result &= test_character (226, 194, 226, 97);


  result &= test_character (195, 195, 227, 65);
  result &= test_character (227, 195, 227, 97);


  result &= test_character (196, 196, 228, 65);
  result &= test_character (228, 196, 228, 97);


  result &= test_character (197, 197, 229, 65);
  result &= test_character (229, 197, 229, 97);


  result &= test_character (198, 198, 230, 65);
  result &= test_character (230, 198, 230, 97);


  result &= test_character (199, 199, 231, 67);
  result &= test_character (231, 199, 231, 99);


  result &= test_character (200, 200, 232, 69);
  result &= test_character (232, 200, 232, 101);


  result &= test_character (201, 201, 233, 69);
  result &= test_character (233, 201, 233, 101);


  result &= test_character (202, 202, 234, 69);
  result &= test_character (234, 202, 234, 101);


  result &= test_character (203, 203, 235, 69);
  result &= test_character (235, 203, 235, 101);


  result &= test_character (204, 204, 236, 73);
  result &= test_character (236, 204, 236, 105);


  result &= test_character (205, 205, 237, 73);
  result &= test_character (237, 205, 237, 105);


  result &= test_character (206, 206, 238, 73);
  result &= test_character (238, 206, 238, 105);


  result &= test_character (207, 207, 239, 73);
  result &= test_character (239, 207, 239, 105);


  result &= test_character (208, 208, 240, 208);
  result &= test_character (240, 208, 240, 240);


  result &= test_character (209, 209, 241, 78);
  result &= test_character (241, 209, 241, 110);


  result &= test_character (210, 210, 242, 79);
  result &= test_character (242, 210, 242, 111);


  result &= test_character (211, 211, 243, 79);
  result &= test_character (243, 211, 243, 111);


  result &= test_character (212, 212, 244, 79);
  result &= test_character (244, 212, 244, 111);


  result &= test_character (213, 213, 245, 79);
  result &= test_character (245, 213, 245, 111);


  result &= test_character (214, 214, 246, 79);
  result &= test_character (246, 214, 246, 111);

  result &= test_character (215, 215, 215, 215);

  result &= test_character (247, 247, 247, 247);

  result &= test_character (216, 216, 248, 79);
  result &= test_character (248, 216, 248, 111);


  result &= test_character (217, 217, 249, 85);
  result &= test_character (249, 217, 249, 117);


  result &= test_character (218, 218, 250, 85);
  result &= test_character (250, 218, 250, 117);


  result &= test_character (219, 219, 251, 85);
  result &= test_character (251, 219, 251, 117);


  result &= test_character (220, 220, 252, 85);
  result &= test_character (252, 220, 252, 117);


  result &= test_character (221, 221, 253, 89);
  result &= test_character (253, 221, 253, 121);


  result &= test_character (222, 222, 254, 222);
  result &= test_character (254, 222, 254, 254);


  result &= test_character (223, 223, 223, 223);

  //Special case for Y with diaeresis as it have the capital latter on
  //within a different category.
  result &= test_character (255, 376, 255, 121);


  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}



static bool
test_latin_A_extended_char_set ()
{
  bool result = true;

  std::cout << "Testing Latin A extended character set...";


  result &= test_character (256, 256, 257, 65);
  result &= test_character (257, 256, 257, 97);


  result &= test_character (258, 258, 259, 65);
  result &= test_character (259, 258, 259, 97);


  result &= test_character (260, 260, 261, 65);
  result &= test_character (261, 260, 261, 97);


  result &= test_character (262, 262, 263, 67);
  result &= test_character (263, 262, 263, 99);


  result &= test_character (264, 264, 265, 67);
  result &= test_character (265, 264, 265, 99);


  result &= test_character (266, 266, 267, 67);
  result &= test_character (267, 266, 267, 99);


  result &= test_character (268, 268, 269, 67);
  result &= test_character (269, 268, 269, 99);


  result &= test_character (270, 270, 271, 68);
  result &= test_character (271, 270, 271, 100);


  result &= test_character (272, 272, 273, 68);
  result &= test_character (273, 272, 273, 100);


  result &= test_character (274, 274, 275, 69);
  result &= test_character (275, 274, 275, 101);


  result &= test_character (276, 276, 277, 69);
  result &= test_character (277, 276, 277, 101);


  result &= test_character (278, 278, 279, 69);
  result &= test_character (279, 278, 279, 101);


  result &= test_character (280, 280, 281, 69);
  result &= test_character (281, 280, 281, 101);


  result &= test_character (282, 282, 283, 69);
  result &= test_character (283, 282, 283, 101);


  result &= test_character (284, 284, 285, 71);
  result &= test_character (285, 284, 285, 103);


  result &= test_character (286, 286, 287, 71);
  result &= test_character (287, 286, 287, 103);


  result &= test_character (288, 288, 289, 71);
  result &= test_character (289, 288, 289, 103);


  result &= test_character (290, 290, 291, 71);
  result &= test_character (291, 290, 291, 103);


  result &= test_character (292, 292, 293, 72);
  result &= test_character (293, 292, 293, 104);


  result &= test_character (294, 294, 295, 72);
  result &= test_character (295, 294, 295, 104);


  result &= test_character (296, 296, 297, 73);
  result &= test_character (297, 296, 297, 105);


  result &= test_character (298, 298, 299, 73);
  result &= test_character (299, 298, 299, 105);


  result &= test_character (300, 300, 301, 73);
  result &= test_character (301, 300, 301, 105);


  result &= test_character (302, 302, 303, 73);
  result &= test_character (303, 302, 303, 105);

  result &= test_character (304, 304, 304, 304);
  result &= test_character (305, 305, 305, 305);

  result &= test_character (306, 306, 307, 73);
  result &= test_character (307, 306, 307, 105);


  result &= test_character (308, 308, 309, 74);
  result &= test_character (309, 308, 309, 106);


  result &= test_character (310, 310, 311, 75);
  result &= test_character (311, 310, 311, 107);

  result &= test_character (312, 312, 312, 312);

  result &= test_character (313, 313, 314, 76);
  result &= test_character (314, 313, 314, 108);


  result &= test_character (315, 315, 316, 76);
  result &= test_character (316, 315, 316, 108);


  result &= test_character (317, 317, 318, 76);
  result &= test_character (318, 317, 318, 108);


  result &= test_character (319, 319, 320, 76);
  result &= test_character (320, 319, 320, 108);


  result &= test_character (321, 321, 322, 76);
  result &= test_character (322, 321, 322, 108);


  result &= test_character (323, 323, 324, 78);
  result &= test_character (324, 323, 324, 110);


  result &= test_character (325, 325, 326, 78);
  result &= test_character (326, 325, 326, 110);


  result &= test_character (327, 327, 328, 78);
  result &= test_character (328, 327, 328, 110);

  result &= test_character (329, 329, 329, 329);

  result &= test_character (330, 330, 331, 78);
  result &= test_character (331, 330, 331, 110);


  result &= test_character (332, 332, 333, 79);
  result &= test_character (333, 332, 333, 111);


  result &= test_character (334, 334, 335, 79);
  result &= test_character (335, 334, 335, 111);


  result &= test_character (336, 336, 337, 79);
  result &= test_character (337, 336, 337, 111);


  result &= test_character (338, 338, 339, 79);
  result &= test_character (339, 338, 339, 111);



  result &= test_character (340, 340, 341, 82);
  result &= test_character (341, 340, 341, 114);


  result &= test_character (342, 342, 343, 82);
  result &= test_character (343, 342, 343, 114);


  result &= test_character (344, 344, 345, 82);
  result &= test_character (345, 344, 345, 114);


  result &= test_character (346, 346, 347, 83);
  result &= test_character (347, 346, 347, 115);


  result &= test_character (348, 348, 349, 83);
  result &= test_character (349, 348, 349, 115);


  result &= test_character (350, 350, 351, 83);
  result &= test_character (351, 350, 351, 115);


  result &= test_character (352, 352, 353, 83);
  result &= test_character (353, 352, 353, 115);


  result &= test_character (354, 354, 355, 84);
  result &= test_character (355, 354, 355, 116);


  result &= test_character (356, 356, 357, 84);
  result &= test_character (357, 356, 357, 116);


  result &= test_character (358, 358, 359, 84);
  result &= test_character (359, 358, 359, 116);


  result &= test_character (360, 360, 361, 85);
  result &= test_character (361, 360, 361, 117);


  result &= test_character (362, 362, 363, 85);
  result &= test_character (363, 362, 363, 117);


  result &= test_character (364, 364, 365, 85);
  result &= test_character (365, 364, 365, 117);


  result &= test_character (366, 366, 367, 85);
  result &= test_character (367, 366, 367, 117);


  result &= test_character (368, 368, 369, 85);
  result &= test_character (369, 368, 369, 117);


  result &= test_character (370, 370, 371, 85);
  result &= test_character (371, 370, 371, 117);


  result &= test_character (372, 372, 373, 87);
  result &= test_character (373, 372, 373, 119);


  result &= test_character (374, 374, 375, 89);
  result &= test_character (375, 374, 375, 121);

  result &= test_character (376, 376, 255, 89);

  result &= test_character (377, 377, 378, 90);
  result &= test_character (378, 377, 378, 122);


  result &= test_character (379, 379, 380, 90);
  result &= test_character (380, 379, 380, 122);


  result &= test_character (381, 381, 382, 90);
  result &= test_character (382, 381, 382, 122);

  result &= test_character (383, 383, 383, 383);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}


static bool
test_latin_B_extended_char_set ()
{
  bool result = true;

  std::cout << "Testing Latin B extended character set...";

  result &= test_character (384, 579, 384, 384);
  result &= test_character (579, 579, 384, 579);

  result &= test_character (385, 385, 385, 385);

  result &= test_character (386, 386, 387, 386);
  result &= test_character (387, 386, 387, 387);

  result &= test_character (388, 388, 389, 388);
  result &= test_character (389, 388, 389, 389);

  result &= test_character (390, 390, 390, 390);

  result &= test_character (391, 391, 392, 67);
  result &= test_character (392, 391, 392, 99);

  result &= test_character (393, 393, 393, 393);
  result &= test_character (394, 394, 394, 394);

  result &= test_character (395, 395, 396, 68);
  result &= test_character (396, 395, 396, 100);

  result &= test_character (397, 397, 397, 397);
  result &= test_character (398, 398, 398, 398);
  result &= test_character (399, 399, 399, 399);
  result &= test_character (400, 400, 400, 400);

  result &= test_character (401, 401, 402, 70);
  result &= test_character (402, 401, 402, 102);

  result &= test_character (403, 403, 403, 403);
  result &= test_character (404, 404, 404, 404);
  result &= test_character (405, 405, 405, 405);
  result &= test_character (406, 406, 406, 406);
  result &= test_character (407, 407, 407, 407);

  result &= test_character (408, 408, 409, 75);
  result &= test_character (409, 408, 409, 107);

  result &= test_character (410, 573, 410, 108);
  result &= test_character (573, 573, 410, 76);


  result &= test_character (411, 411, 411, 411);
  result &= test_character (412, 412, 412, 412);
  result &= test_character (413, 413, 413, 413);

  result &= test_character (414, 544, 414, 414);
  result &= test_character (544, 544, 414, 544);

  result &= test_character (415, 415, 415, 415);

  result &= test_character (416, 416, 417, 79);
  result &= test_character (417, 416, 417, 111);


  result &= test_character (418, 418, 419, 418);
  result &= test_character (419, 418, 419, 419);


  result &= test_character (420, 420, 421, 420);
  result &= test_character (421, 420, 421, 421);

  result &= test_character (422, 422, 422, 422);

  result &= test_character (423, 423, 424, 423);
  result &= test_character (424, 423, 424, 424);

  result &= test_character (425, 425, 425, 425);
  result &= test_character (426, 426, 426, 426);
  result &= test_character (427, 427, 427, 427);

  result &= test_character (428, 428, 429, 84);
  result &= test_character (429, 428, 429, 116);

  result &= test_character (430, 430, 430, 430);

  result &= test_character (431, 431, 432, 85);
  result &= test_character (432, 431, 432, 117);

  result &= test_character (433, 433, 433, 433);
  result &= test_character (434, 434, 434, 434);

  result &= test_character (435, 435, 436, 89);
  result &= test_character (436, 435, 436, 121);


  result &= test_character (437, 437, 438, 90);
  result &= test_character (438, 437, 438, 122);

  result &= test_character (439, 439, 439, 439);

  result &= test_character (440, 440, 441, 440);
  result &= test_character (441, 440, 441, 441);

  result &= test_character (442, 442, 442, 442);
  result &= test_character (443, 443, 443, 443);

  result &= test_character (444, 444, 445, 444);
  result &= test_character (445, 444, 445, 445);

  result &= test_character (446, 446, 446, 446);

  result &= test_character (447, 503, 447, 447);
  result &= test_character (503, 503, 447, 503);

  result &= test_character (448, 448, 448, 448);
  result &= test_character (449, 449, 449, 449);
  result &= test_character (450, 450, 450, 450);
  result &= test_character (451, 451, 451, 451);

  result &= test_character (452, 452, 454, 68);
  result &= test_character (453, 453, 454, 68);
  result &= test_character (454, 453, 454, 100);


  result &= test_character (455, 455, 457, 76);
  result &= test_character (456, 456, 457, 76);
  result &= test_character (457, 456, 457, 108);


  result &= test_character (458, 458, 460, 78);
  result &= test_character (459, 459, 460, 78);
  result &= test_character (460, 459, 460, 110);


  result &= test_character (461, 461, 462, 65);
  result &= test_character (462, 461, 462, 97);


  result &= test_character (463, 463, 464, 73);
  result &= test_character (464, 463, 464, 105);


  result &= test_character (465, 465, 466, 79);
  result &= test_character (466, 465, 466, 111);


  result &= test_character (467, 467, 468, 85);
  result &= test_character (468, 467, 468, 117);


  result &= test_character (469, 469, 470, 85);
  result &= test_character (470, 469, 470, 117);


  result &= test_character (471, 471, 472, 85);
  result &= test_character (472, 471, 472, 117);


  result &= test_character (473, 473, 474, 85);
  result &= test_character (474, 473, 474, 117);


  result &= test_character (475, 475, 476, 85);
  result &= test_character (476, 475, 476, 117);

  result &= test_character (477, 477, 477, 477);

  result &= test_character (478, 478, 479, 65);
  result &= test_character (479, 478, 479, 97);


  result &= test_character (480, 480, 481, 65);
  result &= test_character (481, 480, 481, 97);


  result &= test_character (482, 482, 483, 65);
  result &= test_character (483, 482, 483, 97);


  result &= test_character (484, 484, 485, 71);
  result &= test_character (485, 484, 485, 103);


  result &= test_character (486, 486, 487, 71);
  result &= test_character (487, 486, 487, 103);


  result &= test_character (488, 488, 489, 75);
  result &= test_character (489, 488, 489, 107);


  result &= test_character (490, 490, 491, 79);
  result &= test_character (491, 490, 491, 111);


  result &= test_character (492, 492, 493, 79);
  result &= test_character (493, 492, 493, 111);


  result &= test_character (494, 494, 495, 494);
  result &= test_character (495, 494, 495, 495);

  result &= test_character (496, 496, 496, 496);

  result &= test_character (497, 497, 499, 68);
  result &= test_character (498, 498, 499, 68);
  result &= test_character (499, 498, 499, 100);


  result &= test_character (500, 500, 501, 71);
  result &= test_character (501, 500, 501, 103);

  result &= test_character (502, 502, 502, 502);

  result &= test_character (504, 504, 505, 78);
  result &= test_character (505, 504, 505, 110);


  result &= test_character (506, 506, 507, 65);
  result &= test_character (507, 506, 507, 97);


  result &= test_character (508, 508, 509, 65);
  result &= test_character (509, 508, 509, 97);


  result &= test_character (510, 510, 511, 79);
  result &= test_character (511, 510, 511, 111);


  result &= test_character (512, 512, 513, 65);
  result &= test_character (513, 512, 513, 97);


  result &= test_character (514, 514, 515, 65);
  result &= test_character (515, 514, 515, 97);


  result &= test_character (516, 516, 517, 69);
  result &= test_character (517, 516, 517, 101);


  result &= test_character (518, 518, 519, 69);
  result &= test_character (519, 518, 519, 101);


  result &= test_character (520, 520, 521, 73);
  result &= test_character (521, 520, 521, 105);


  result &= test_character (522, 522, 523, 73);
  result &= test_character (523, 522, 523, 105);


  result &= test_character (524, 524, 525, 79);
  result &= test_character (525, 524, 525, 111);


  result &= test_character (526, 526, 527, 79);
  result &= test_character (527, 526, 527, 111);


  result &= test_character (528, 528, 529, 82);
  result &= test_character (529, 528, 529, 114);


  result &= test_character (530, 530, 531, 82);
  result &= test_character (531, 530, 531, 114);


  result &= test_character (532, 532, 533, 85);
  result &= test_character (533, 532, 533, 117);


  result &= test_character (534, 534, 535, 85);
  result &= test_character (535, 534, 535, 117);


  result &= test_character (536, 536, 537, 83);
  result &= test_character (537, 536, 537, 115);


  result &= test_character (538, 538, 539, 84);
  result &= test_character (539, 538, 539, 116);


  result &= test_character (540, 540, 541, 540);
  result &= test_character (541, 540, 541, 541);


  result &= test_character (542, 542, 543, 72);
  result &= test_character (543, 542, 543, 104);

  result &= test_character (545, 545, 545, 545);

  result &= test_character (546, 546, 547, 546);
  result &= test_character (547, 546, 547, 547);


  result &= test_character (548, 548, 549, 90);
  result &= test_character (549, 548, 549, 122);


  result &= test_character (550, 550, 551, 65);
  result &= test_character (551, 550, 551, 97);


  result &= test_character (552, 552, 553, 69);
  result &= test_character (553, 552, 553, 101);


  result &= test_character (554, 554, 555, 79);
  result &= test_character (555, 554, 555, 111);


  result &= test_character (556, 556, 557, 79);
  result &= test_character (557, 556, 557, 111);


  result &= test_character (558, 558, 559, 79);
  result &= test_character (559, 558, 559, 111);


  result &= test_character (560, 560, 561, 79);
  result &= test_character (561, 560, 561, 111);


  result &= test_character (562, 562, 563, 89);
  result &= test_character (563, 562, 563, 121);

  result &= test_character (564, 564, 564, 564);
  result &= test_character (565, 565, 565, 565);
  result &= test_character (566, 566, 566, 566);
  result &= test_character (567, 567, 567, 567);

  result &= test_character (568, 568, 569, 568);
  result &= test_character (569, 568, 569, 569);

  result &= test_character (570, 570, 570, 570);

  result &= test_character (571, 571, 572, 67);
  result &= test_character (572, 571, 572, 99);

  result &= test_character (574, 574, 574, 574);
  result &= test_character (575, 575, 575, 575);
  result &= test_character (576, 576, 576, 576);

  result &= test_character (577, 577, 578, 577);
  result &= test_character (578, 577, 578, 578);

  result &= test_character (580, 580, 580, 580);
  result &= test_character (581, 581, 581, 581);

  result &= test_character (582, 582, 583, 69);
  result &= test_character (583, 582, 583, 101);

  result &= test_character (584, 584, 585, 74);
  result &= test_character (585, 584, 585, 106);

  result &= test_character (586, 586, 587, 81);
  result &= test_character (587, 586, 587, 113);

  result &= test_character (588, 588, 589, 82);
  result &= test_character (589, 588, 589, 114);


  result &= test_character (590, 590, 591, 89);
  result &= test_character (591, 590, 591, 121);


  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}

static bool
test_latin_IPA_char_set ()
{
  bool result = true;

  std::cout << "Testing IPA character set...";

  for (uint32_t i = 592; (i < 884) && result; ++i)
    result &= test_character (i, i, i, i);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}


static bool
test_greek_char_set ()
{
  bool result = true;

  std::cout << "Testing Greek extended character set...";

  for (uint32_t i = 0x374; (i < 0x386) && result; ++i)
    result &= test_character (i, i, i, i);

  result &= test_character (0x386, 0x386, 0x3AC, 0x386);
  result &= test_character (0x3AC, 0x386, 0x3AC, 0x3AC);

  result &= test_character (0x387, 0x387, 0x387, 0x387);

  for (uint32_t i = 0x388; (i < 0x38B) && result; ++i)
    {
      result &= test_character (i, i, i + 0x25, i);
      result &= test_character (i + 0x25, i, i + 0x25, i + 0x25);
    }

  result &= test_character (0x38B, 0x38B, 0x38B, 0x38B);

  result &= test_character (0x38C, 0x38C, 0x3CC, 0x38C);
  result &= test_character (0x3CC, 0x38C, 0x3CC, 0x3CC);

  result &= test_character (0x38D, 0x38D, 0x38D, 0x38D);

  result &= test_character (0x38E, 0x38E, 0x3CD, 0x38E);
  result &= test_character (0x3CD, 0x38E, 0x3CD, 0x3CD);

  result &= test_character (0x38F, 0x38F, 0x3CE, 0x38F);
  result &= test_character (0x3CE, 0x38F, 0x3CE, 0x3CE);

  result &= test_character (0x390, 0x390, 0x390, 0x390);

  for (uint32_t i = 0x391; (i < 0x3A2) && result; ++i)
    {
      result &= test_character (i, i, i + 0x20, i);
      result &= test_character (i + 0x20, i, i + 0x20, i + 0x20);
    }

  result &= test_character (0x3A2, 0x3A2, 0x3A2, 0x3A2);
  result &= test_character (0x3C2, 0x3C2, 0x3C2, 0x3C2);

  for (uint32_t i = 0x3A3; (i < 0x3AC) && result; ++i)
    {
      result &= test_character (i, i, i + 0x20, i);
      result &= test_character (i + 0x20, i, i + 0x20, i + 0x20);
    }

  result &= test_character (0x3B0, 0x3B0, 0x3B0, 0x3B0);
  result &= test_character (0x3CF, 0x3CF, 0x3CF, 0x3CF);

  for (uint32_t i = 0x3D0; (i < 0x3E2) && result; ++i)
    result &= test_character (i, i, i, i);

  for (uint32_t i = 0x3E2; (i < 0x3F0) && result; i += 2)
    {
      result &= test_character (i, i, i + 1, i);
      result &= test_character (i + 1, i, i + 1, i + 1);
    }

  for (uint32_t i = 0x3F0; (i < 0x400) && result; ++i)
    result &= test_character (i, i, i, i);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}


static bool
test_cyrillic_char_set ()
{
  bool result = true;

  std::cout << "Testing Cyrillic character set...";

  for (uint32_t i = 0x400; (i < 0x410) && result; ++i)
    {
      result &= test_character (i, i, i + 0x50, i);
      result &= test_character (i + 0x50, i, i + 0x50, i + 0x50);
    }

  for (uint32_t i = 0x410; (i < 0x430) && result; ++i)
    {
      result &= test_character (i, i, i + 0x20, i);
      result &= test_character (i + 0x20, i, i + 0x20, i + 0x20);
    }

  for (uint32_t i = 0x460; (i < 0x481) && result; i += 2)
    {
      result &= test_character (i, i, i + 1, i);
      result &= test_character (i + 1, i, i + 1, i + 1);
    }

  for (uint32_t i = 0x482; (i < 0x48A) && result; ++i)
    result &= test_character (i, i, i, i);

  for (uint32_t i = 0x48A; (i < 0x4FF) && result; i += 2)
    {
      result &= test_character (i, i, i + 1, i);
      result &= test_character (i + 1, i, i + 1, i + 1);
    }

  for (uint32_t i = 0x500; (i < 0x527) && result; i += 2)
    {
      result &= test_character (i, i, i + 1, i);
      result &= test_character (i + 1, i, i + 1, i + 1);
    }

  for (uint32_t i = 0x528; (i < 0x530) && result; ++i)
    result &= test_character (i, i, i, i);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}

static bool
test_armenian_char_set ()
{
  bool result = true;

  std::cout << "Testing Armenian character set...";

  result &= test_character (0x530, 0x530, 0x530, 0x530);

  for (uint32_t i = 0x531; (i < 0x557) && result; ++i)
    {
      result &= test_character (i, i, i + 0x30, i);
      result &= test_character (i + 0x30, i, i + 0x30, i + 0x30);
    }

  for (uint32_t i = 0x557; (i < 0x561) && result; ++i)
    result &= test_character (i, i, i, i);

  for (uint32_t i = 0x587; (i < 0x5BE) && result; ++i)
    result &= test_character (i, i, i, i);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}

static bool
test_unicode_rest ()
{
  bool result = true;

  std::cout << "Testing the rest of Unicode character set...";

  result &= test_character (0x530, 0x530, 0x530, 0x530);


  for (uint32_t i = 0x5BE; (i < 0xD800) && result; ++i)
    result &= test_character (i, i, i, i);

  for (uint32_t i = 0xE000; (i < 0x110000) && result; ++i)
    result &= test_character (i, i, i, i);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}


static bool
test_unicode_evaluation ()
{
  bool result = true;

  std::cout << "Testing if all Unicode chars have been evaluated...";

  for (uint32_t i = 0; (i < 0xD800) && result; ++i)
    result &= sVerifiedChars.test (i);

  for (uint32_t i = 0xD800; (i < 0xE000) && result; ++i)
      result &= ! sVerifiedChars.test (i);

  for (uint32_t i = 0xE000; (i < 0x110000) && result; ++i)
    result &= sVerifiedChars.test (i);

  if (result)
    std::cout << "PASS\n";

  else
    std::cout << "FAIL\n";

  return result;
}


int
main (int argc, char** argv)
{
  bool success = true;

  DBSInit (DBSSettings ());

  sVerifiedChars.reset ();

  success = success & test_latin_base_char_set();
  success = success & test_latin_1_supplement_char_set ();
  success = success & test_latin_A_extended_char_set ();
  success = success & test_latin_B_extended_char_set ();
  success = success & test_latin_IPA_char_set();
  success = success & test_greek_char_set ();
  success = success & test_cyrillic_char_set();
  success = success & test_armenian_char_set();
  success = success & test_unicode_rest ();
  success = success & test_unicode_evaluation ();


  DBSShoutdown ();
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
