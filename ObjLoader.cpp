/*  STLover - A powerful tool for viewing and manipulating 3D STL models
 *  Copyright (C) 2020 Gerasim Troeglazov <3dEyes@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "ObjLoader.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

vector<string> split(string line,char sep=' ')
{
	vector<string> tokens;

	bool knee=false;
	string tmp;

	for (char c : line) {

		if (c==sep) {
			tokens.push_back(tmp);
			tmp="";
		}
		else {
			tmp=tmp+c;
		}
	}

	tokens.push_back(tmp);

	return tokens;
}

stl_file* LoadObj(const char* filename)
{
	fstream fs;

	setlocale(LC_NUMERIC,"C");

	fs.open(filename,fstream::in);

	vector<stl_vertex> vertices;
	vector<stl_normal> normals;
	vector<stl_facet> faces;

	while (!fs.eof()) {
		string line;
		getline(fs,line);
		
		vector<string> tmp = split(line);
		
		if (tmp.size()<=0) {
			continue;
		}
		
		if (tmp[0]=="v") {
			stl_vertex v;
			
			v.x=stof(tmp[1]);
			v.y=stof(tmp[2]);
			v.z=stof(tmp[3]);
			
			vertices.push_back(v);
		}
		
		if (tmp[0]=="vn") {
			stl_normal vn;
			
			vn.x=stof(tmp[1]);
			vn.y=stof(tmp[2]);
			vn.z=stof(tmp[3]);
			
			normals.push_back(vn);
		}
		
		if (tmp[0]=="f") {
			stl_facet facet;

			vector<string> tmp2 = split(tmp[1],'/');
			facet.vertex[0] = vertices[stoi(tmp2[0])-1];
			
			tmp2 = split(tmp[2],'/');
			facet.vertex[1] = vertices[stoi(tmp2[0])-1];
			
			tmp2 = split(tmp[3],'/');
			facet.vertex[2] = vertices[stoi(tmp2[0])-1];
			
			// Calculate normal
			float normal[3];
			stl_calculate_normal(normal, &facet);
			stl_normalize_vector(normal);
			facet.normal.x = normal[0];
			facet.normal.y = normal[1];
			facet.normal.z = normal[2];

			faces.push_back(facet);
		}
	}
	
	fs.close();
	
	stl_file* stl = new stl_file();
	stl_initialize(stl);

	stl->stats.number_of_facets = faces.size();
	stl->stats.original_num_facets = faces.size();
	stl_allocate(stl);
	
	for (size_t n=0;n<faces.size();n++) {
		stl->facet_start[n] = faces[n];
	}
	
	return stl;
}
