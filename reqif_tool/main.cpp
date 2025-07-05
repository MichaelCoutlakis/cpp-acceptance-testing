/******************************************************************************
 * Copyright 2025 Michael Coutlakis
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <pugixml.hpp>

#include <iostream>

namespace reqif
{
class data_type
{
public:
    std::string m_id;
    std::string m_name;
};

class attr_def
{
public:
    std::string m_id;
    std::string m_name;
    std::string m_data_type;
};

class spec_type
{
public:
    std::string m_id;
    std::string m_name;
    std::vector<attr_def> m_attr_defs;
};

class attr
{
public:
    std::string m_defn;  //!< Reference to the attribute definition
    std::string m_value; //!< Attribute contents
};

class spec_obj
{
public:
    std::string m_id;
    std::string m_type;
    std::vector<attr> m_values;
};

class spec_relation
{
public:
    std::string m_id;
    std::string m_source;
    std::string m_target;
    std::string m_type;
};

class spec_entry
{
public:
    std::string m_spec_obj;
    std::vector<spec_entry> m_children;
};

class specification
{
    std::string m_id;
    std::string m_name;
    std::vector<spec_entry> m_children;
};

class file
{
public:
    std::map<std::string, data_type> m_data_types;
    std::map<std::string, spec_type> m_spec_types;
    std::map<std::string, spec_relation> m_spec_relations;
    std::map<std::string, spec_obj> m_spec_objs;
};

std::string get_xml_content(const pugi::xml_node &node)
{
    std::ostringstream os;
    for(auto &child : node.children())
        child.print(os, "", pugi::format_raw);
    return os.str();
}

std::vector<attr> parse_attributes(const pugi::xml_node &attr_values_node)
{
    std::vector<attr> attrs;

    for(auto &v : attr_values_node.children())
    {
        std::string attr_class = v.name();

        reqif::attr a;
        a.m_defn = v.child("DEFINITION").first_child().text().as_string();

        if(attr_class == "ATTRIBUTE-VALUE-STRING")
            a.m_value = v.attribute("THE-VALUE").as_string();
        else if(attr_class == "ATTRIBUTE-VALUE-XHTML")
            a.m_value = get_xml_content(v.child("THE-VALUE"));

        else if(attr_class == "ATTRIBUTE-VALUE-ENUMERATION")
        {
            for(auto &e : v.children("VALUES"))
            {
                // TODO
            }
        }
        else
            throw std::runtime_error("Unhandled attribute type: " + attr_class);

        attrs.push_back(a);
    }
    return attrs;
}

file parse(const std::string &filename)
{
    pugi::xml_document xml;
    if(!xml.load_file(filename.c_str()))
        throw std::runtime_error("Failed to load ReqIF file " + filename);

    auto reqif_node = xml.child("REQ-IF");
    auto reqif_content_node = reqif_node.child("CORE-CONTENT").child("REQ-IF-CONTENT");

    reqif::file reqif_file;

    // Parse the data types:
    for(auto &d : reqif_content_node.child("DATATYPES").children())
    {
        reqif::data_type dt;
        dt.m_id = d.attribute("IDENTIFIER").as_string();
        dt.m_name = d.attribute("LONG-NAME").as_string();

        reqif_file.m_data_types[dt.m_id] = dt;
    }

    // Load the SPEC-TYPES:
    for(auto &st_node : reqif_content_node.child("SPEC-TYPES").children())
    {
        reqif::spec_type st;
        st.m_id = st_node.attribute("IDENTIFIER").as_string();
        st.m_name = st_node.attribute("LONG-NAME").as_string();

        // Load the attributes:
        for(auto &attr : st_node.child("SPEC-ATTRIBUTES").children())
        {
            reqif::attr_def a;
            a.m_id = attr.attribute("IDENTIFIER").as_string();
            a.m_name = attr.attribute("LONG-NAME").as_string();
            a.m_data_type = attr.child("TYPE").first_child().text().as_string(); // Only one child
            st.m_attr_defs.push_back(a);
        }
        reqif_file.m_spec_types[st.m_id] = st;
    }

    // Load the spec objects:
    // auto spec_objs_node = reqif_node.hash_value
    for(auto &o : reqif_content_node.child("SPEC-OBJECTS").children())
    {
        reqif::spec_obj so;
        so.m_id = o.attribute("IDENTIFIER").as_string();
        so.m_type = o.child("TYPE").child("SPEC-OBJECT-TYPE-REF").text().as_string();
        so.m_values = parse_attributes(o.child("VALUES"));
        reqif_file.m_spec_objs[so.m_id] = so;
    }

    // Load the spec relations:
    for(auto &r : reqif_content_node.child("SPEC-RELATIONS").children("SPEC-RELATION"))
    {
        reqif::spec_relation sr;
        sr.m_id = r.attribute("IDENTIFIER").as_string();
        sr.m_target = r.child("TARGET").child("SPEC-OBJECT-REF").text().as_string();
        sr.m_source = r.child("SOURCE").child("SPEC-OBJECT-REF").text().as_string();
        reqif_file.m_spec_relations[sr.m_id] = sr;
    }

    // Load the specifications:
    for(auto &spec_node : reqif_content_node.child("SPECIFICATIONS").children()) { }

    return reqif_file;
}
}

int main(int argc, char **argv)
{
    std::string reqif_filename{"ReqIF/cpp-acceptance-testing.reqif"}; // Input ReqIF file
    std::string cpp_output_filename{"../tests/reqif-spec.cpp"};
    std::string doc_output_filename{"../tests/reqif-doc.html"};

    // Traceability from this spec object type name,e.g. the Spec Object defining test cases
    std::string trace_from_spec_obj_tn{"TestCase"};
    // Trace back up to this spec object type, e.g. the user requirements.
    std::string trace_to_spec_obj_tn{"Requirement Type"};

    auto reqif_file = reqif::parse(reqif_filename);

    // Now we can make a list of all of the source - target links:
    std::vector<std::pair<std::string, std::string>> traces;

    // for (auto &[key, val] : reqif_file.m_spec_objs)
    //{
    //     if (val.m_type == it_source_type->second.m_id)
    //     {
    //         // Found a potential source type, see if it is associated with any relations:
    //         auto it_relation = std::find_if(
    //             reqif_file.m_spec_relations.begin(),
    //             reqif_file.m_spec_relations.end(),
    //             [&](const auto &key_val) { return key_val.second.m_source == val.m_id; });

    //        auto it_target = reqif_file.m_spec_objs.end();

    //        if (it_relation != reqif_file.m_spec_relations.end())
    //            it_target = std::find_if(
    //                reqif_file.m_spec_objs.begin(),
    //                reqif_file.m_spec_objs.end(),
    //                [&](const auto &key_val)
    //                { return key_val.second.m_id == it_relation->second.m_target; });

    //        if (it_target != reqif_file.m_spec_objs.end())
    //        {
    //            // Can continue:
    //            int asf = 3;
    //        }
    //    }
    //}

    return 0;
}