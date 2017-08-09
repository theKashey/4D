/***************************************************************************
                          XMLmsg.h  -  description
                             -------------------
    begin                : Thu Jul 3 2003
    copyright            : (C) 2003 by kashey
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _XMLMSG_
#define _XMLMSG_

 WORD MSG2XML(LPSTR BUFFER,const BlopMessage &msg);
 bool XML2MSG(const LPSTR BUFFER,BlopMessage &msg);

#endif

