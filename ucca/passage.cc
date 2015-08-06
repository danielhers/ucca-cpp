//
// Created by danielh on 7/30/15.
//
#include <iostream>
#include <vector>
#include <string.h>

#include <rapidxml/rapidxml_print.hpp>
#include <rapidxml/rapidxml.hpp>

#include "ucca/passage.h"

using namespace std;
using namespace rapidxml;
using namespace ucca;

Passage::Passage(unsigned id)
    : id(id), annotation_id(0)
{
  layers[0] = new Layer(0);
  layers[1] = new Layer(1);
}

Passage* Passage::load(istream& in)
{
  Passage *p = new Passage();
  xml_document<> doc;
  vector<char> buffer((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
  buffer.push_back('\0');
  doc.parse<0>(&buffer[0]);
  xml_node<> *root = doc.first_node("root");
  for (xml_attribute<> *attr = root->first_attribute(); attr; attr = attr->next_attribute())
  {
    if (strcmp(attr->name(), "passageID") == 0) {
      p->id = stoi(attr->value());
    } else if (strcmp(attr->name(), "annotationID") == 0) {
      p->annotation_id = stoi(attr->value());
    } else {
      throw xml_exception(string("Unknown passage attribute \"") + attr->name() + "\"");
    }
  }
  for (xml_node<> *layer_node = root->first_node("layer"); layer_node; layer_node = layer_node->next_sibling("layer"))
  {
    unsigned layer_id = 0;
    for (xml_attribute<> *attr = layer_node->first_attribute(); attr; attr = attr->next_attribute())
    {
      if (strcmp(attr->name(), "layerID") == 0) {
        layer_id = stoi(attr->value());
      } else {
        throw xml_exception(string("Unknown layer attribute \"") + attr->name() + "\"");
      }
    }
    Layer *layer = new Layer(layer_id);
    p->layers[layer_id] = layer;
    for (xml_node<> *node_node = layer_node->first_node("node"); node_node; node_node = node_node->next_sibling("node"))
    {
      string node_id = "";
      string node_type = "";
      for (xml_attribute<> *attr = node_node->first_attribute(); attr; attr = attr->next_attribute())
      {
        if (strcmp(attr->name(), "ID") == 0) {
          node_id = attr->value();
        } else if (strcmp(attr->name(), "type") == 0) {
          node_type = attr->value();
        } else {
          throw xml_exception(string("Unknown node attribute \"") + attr->name() +
                                  "\" in layer " + to_string(layer_id));
        }
      }
      Node *node = new Node(node_id, node_type);
      p->nodes[node_id] = node;
      layer->nodes[node_id] = node;
      for (xml_node<> *attributes_node = node_node->first_node("attributes"); attributes_node;
           attributes_node = attributes_node->next_sibling("attributes"))
      {
        for (xml_attribute<> *attr = attributes_node->first_attribute(); attr; attr = attr->next_attribute())
        {
          if (strcmp(attr->name(), "paragraph") == 0) {
            node->paragraph = stoi(attr->value());
          } else if (strcmp(attr->name(), "paragraph_position") == 0) {
            node->paragraph_position = stoi(attr->value());
          } else if (strcmp(attr->name(), "text") == 0) {
            node->text = attr->value();
          } else if (strcmp(attr->name(), "implicit") == 0) {
            node->implicit = (strcmp(attr->value(), "True") == 0);
          } else if (strcmp(attr->name(), "uncertain") == 0) {
            node->uncertain = (strcmp(attr->value(), "True") == 0);
          } else {
            throw xml_exception(string("Unknown node optional attribute \"") + attr->name() + "\" in node " + node_id);
          }
        }
      } // node attributes
      for (xml_node<> *extra_node = node_node->first_node("extra"); extra_node;
           extra_node = extra_node->next_sibling("extra"))
      {
        for (xml_attribute<> *attr = extra_node->first_attribute(); attr; attr = attr->next_attribute())
        {
          if (strcmp(attr->name(), "remarks") == 0) {
            node->remarks = attr->value();
          } else {
            throw xml_exception(string("Unknown node extra attribute \"") + attr->name() + "\" in node " + node_id);
          }
        }
      } // node extra
      for (xml_node<> *edge_node = node_node->first_node("edge"); edge_node;
           edge_node = edge_node->next_sibling("edge"))
      {
        string edge_to_id = "";
        string edge_type = "";
        for (xml_attribute<> *attr = edge_node->first_attribute(); attr; attr = attr->next_attribute())
        {
          if (strcmp(attr->name(), "toID") == 0) {
            edge_to_id = attr->value();
          } else if (strcmp(attr->name(), "type") == 0) {
            edge_type = attr->value();
          } else {
            throw xml_exception(string("Unknown edge attribute \"") + attr->name() + "\" in node " + node_id);
          }
        }
        Edge *edge = new Edge(node, edge_to_id, edge_type);
        p->edges[make_pair(node_id, edge_to_id)] = edge;
        layer->edges[make_pair(node_id, edge_to_id)] = edge;
        node->edges[edge_to_id] = edge;
        for (xml_node<> *attributes_node = edge_node->first_node("attributes"); attributes_node;
             attributes_node = attributes_node->next_sibling("attributes"))
        {
          for (xml_attribute<> *attr = attributes_node->first_attribute(); attr; attr = attr->next_attribute())
          {
            if (strcmp(attr->name(), "remote") == 0) {
              edge->remote = (strcmp(attr->value(), "True") == 0);
            } else {
              throw xml_exception(string("Unknown edge optional attribute \"") + attr->name() + "\" in edge from " +
                                      node_id + " to " + edge_to_id);
            }
          }
        } // edge attributes
      } // edge
    } // node
  } // layer
  for(auto it = p->edges.begin(); it != p->edges.end(); ++it) {
    it->second->to = p->nodes[it->second->to_id];
  }
  return p;
}

Passage::~Passage() {
  for(auto it = layers.begin(); it != layers.end(); ++it) delete it->second;
  for(auto it = nodes.begin() ; it != nodes.end() ; ++it) delete it->second;
  for(auto it = edges.begin() ; it != edges.end() ; ++it) delete it->second;
}

static const char* str(xml_document<>& doc, const string& s) {
  return doc.allocate_string(s.c_str());
}

static const char* str(xml_document<>& doc, const unsigned& n) {
  return str(doc, to_string(n));
}

void Passage::save(ostream& os) const {
  xml_document<> doc;
  xml_node<> *root = doc.allocate_node(node_element, "root");
  doc.append_node(root);
  root->append_attribute(doc.allocate_attribute("annotationID", str(doc, annotation_id)));
  root->append_attribute(doc.allocate_attribute("passageID", str(doc, id)));
  root->append_node(doc.allocate_node(node_element, "attributes"));
  for(map<unsigned, Layer*>::const_iterator layer_it = layers.begin(); layer_it != layers.end(); ++layer_it) {
    xml_node<> *layer = doc.allocate_node(node_element, "layer");
    root->append_node(layer);
    layer->append_attribute(doc.allocate_attribute("layerID", str(doc, layer_it->second->id)));
    layer->append_node(doc.allocate_node(node_element, "attributes"));
    for(map<string, Node*>::const_iterator node_it = layer_it->second->nodes.begin();
        node_it != layer_it->second->nodes.end(); ++node_it) {
      xml_node<> *node = doc.allocate_node(node_element, "node");
      layer->append_node(node);
      node->append_attribute(doc.allocate_attribute("ID", str(doc, node_it->second->id)));
      node->append_attribute(doc.allocate_attribute("type", str(doc, node_it->second->type)));
      xml_node<> *attributes = doc.allocate_node(node_element, "attributes");
      node->append_node(attributes);
      if (node_it->second->paragraph > 0) {
        attributes->append_attribute(
            doc.allocate_attribute("paragraph", str(doc, node_it->second->paragraph)));
      }
      if (node_it->second->paragraph_position > 0) {
        attributes->append_attribute(
            doc.allocate_attribute("paragraph_position", str(doc, node_it->second->paragraph_position)));
      }
      if (!node_it->second->text.empty()) {
        attributes->append_attribute(doc.allocate_attribute("text", str(doc, node_it->second->text)));
      }
      if (node_it->second->implicit) {
        attributes->append_attribute(doc.allocate_attribute("implicit", "True"));
      }
      if (node_it->second->uncertain) {
        attributes->append_attribute(doc.allocate_attribute("uncertain", "True"));
      }
      if (!node_it->second->remarks.empty()) {
        xml_node<> *extra = doc.allocate_node(node_element, "extra");
        node->append_node(extra);
        extra->append_attribute(doc.allocate_attribute("remarks", str(doc, node_it->second->remarks)));
      }
      for(map<string, Edge*>::const_iterator edge_it = node_it->second->edges.begin();
          edge_it != node_it->second->edges.end(); ++edge_it) {
        xml_node<> *edge = doc.allocate_node(node_element, "edge");
        node->append_node(edge);
        edge->append_attribute(doc.allocate_attribute("toID", str(doc, edge_it->second->to_id)));
        edge->append_attribute(doc.allocate_attribute("type", str(doc, edge_it->second->type)));
        xml_node<> *attributes = doc.allocate_node(node_element, "attributes");
        edge->append_node(attributes);
        if (edge_it->second->remote) {
          attributes->append_attribute(doc.allocate_attribute("remote", "True"));
        }
      }
    }
  }
  os << doc;
}

Layer::Layer(unsigned id)
    : id(id) {}

Layer::~Layer() {}

Node::Node(string id, string type)
    : id(id), type(type),
      paragraph(0), paragraph_position(0), text(""),
      implicit(false), uncertain(false), remarks("") {}

Node::~Node() {}

Edge::Edge(Node* from, string to_id, string type)
    : type(type), remote(false), from(from), to_id(to_id) {}

Edge::~Edge() {}
