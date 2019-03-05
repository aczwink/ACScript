/*
* Copyright (c) 2018-2019 Amir Czwink (amir130@hotmail.de)
*
* This file is part of ACScript.
*
* ACScript is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ACScript is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ACScript.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

namespace ACSB
{
	class Procedure
	{
	public:
		//Constructor
		inline Procedure()
		{
		}

		inline Procedure(uint16 nLocals, uint32 offset, uint32 size) : nLocals(nLocals), offset(offset), size(size)
		{
		}

		//Inline
		inline uint16 GetNumberOfLocals() const
		{
			return this->nLocals;
		}

		inline uint32 GetOffset() const
		{
			return this->offset;
		}

	private:
		//Members
		uint16 nLocals;
		uint32 offset;
		uint32 size;
	};
}