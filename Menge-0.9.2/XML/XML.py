from xml.dom.minidom import parse
import numpy as np

def readXML():
    domTree = parse("./Town1.xml")
    # 文档根元素
    rootNode = domTree.documentElement
    print(rootNode.nodeName)

    # 所有顾客
    GoalSets = rootNode.getElementsByTagName("GoalSet")
    print("**** GoalSet ****")
    i=1
    for GoalSet in GoalSets:
        if GoalSet.hasAttribute("id"):
            print("id:", GoalSet.getAttribute("id"))
            GoalSet.setAttribute('id', str(i))
            print("id:", GoalSet.getAttribute("id"))
            i = i+2
            # name 元素
            '''
            name = GoalSets.getElementsByTagName("name")[0]
            print(name.nodeName, ":", name.childNodes[0].data)
            # phone 元素
            phone = GoalSets.getElementsByTagName("phone")[0]
            print(phone.nodeName, ":", phone.childNodes[0].data)
            # comments 元素
            comments = GoalSets.getElementsByTagName("comments")[0]
            print(comments.nodeName, ":", comments.childNodes[0].data)
            '''

def setXML():
    domTree = parse("./Town_v2B_11.xml")
    rootNode = domTree.documentElement
    GoalSets = rootNode.getElementsByTagName("GoalSet")
    Goals = rootNode.getElementsByTagName("Goal")
    i=1
    k=1
    for GoalSet in GoalSets:
        GoalSet.setAttribute('id', str(i))
        i = i + 1
    for Goal in Goals:
        Goal.setAttribute('id', str(k))
        k = k + 1

    with open('D:\\File\\Project\\Menge-master\\XML\\Town_v2B_22', 'w') as f:
        # 缩进 - 换行 - 编码
        domTree.writexml(f, addindent='  ', encoding='utf-8')

if __name__ == '__main__':
   # readXML()
    setXML()
