import xml.etree.ElementTree as ET
from xml.dom import minidom

def subElement(root, tag,x,y):
    ele = ET.SubElement(root, tag)
    ele.set("p_x",x)
    ele.set("p_y",y)
    ele.tail = '\n\t\t'


def saveXML(root, filename, indent="\t", newl="\n", encoding="UTF-8"):
    rawText = ET.tostring(root)
    dom = minidom.parseString(rawText)
    f = open(filename, 'w')  # w替换为a，追加
    dom.writexml(f, indent=indent, newl=newl, encoding=encoding)
    f.close()