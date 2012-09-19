/******************************************************************************
WHISPER - An advanced database system
Copyright (C) 2008  Iulian Popa

Address: Str Olimp nr. 6
         Pantelimon Ilfov,
         Romania
Phone:   +40721939650
e-mail:  popaiulian@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef SERVER_PROTOCOL_H_
#define SERVER_PROTOCOL_H_

#define FRAME_HDR_SIZE_OFF              0x00
#define FRAME_HDR_ENCTYPE_OFF           0x02
#define FRAME_HDR_FRMID                 0x04
#define FRAME_HDR_CNTRES_OFF            0x08
#define FRAME_HDR_SRVRES_OFF            0x0C
#define FRAME_HDR_DATA_OFF              0x10

#define FRAME_ENCTYPE_PLAIN             0x0001
#define FRAME_ENCTYPE_SHKEY             0x0002
#define FRAME_ENCTYPE_BUSSY             0xFFFF


#endif /* SERVER_PROTOCOL_H_ */
