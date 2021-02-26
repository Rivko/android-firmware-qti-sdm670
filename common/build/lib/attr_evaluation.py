'''
=======================================================================================================
attr_evaluation.py: BinaryTree Library

Usage:
   This script contains code for creating a binary tree for an attribute string and 
   evaluating the file element's attributes

 Copyright (c) 2011-2017 by Qualcomm Technologies, Incorporated. All Rights Reserved.
 QUALCOMM Proprietary/GTDR
 
-----------------------------------------------------------------------------------------------
 $Header: //components/rel/build.glue/1.0/lib/attr_evaluation.py#2 $
 $DateTime: 2017/09/20 18:48:25 $
======================================================================================================
'''

class stack:
   
   def __init__(self):
      self.top = 0
      self.a = []
    
   def push(self, data):
      # print "push:",data
      self.a.append(data)
    
   def pop(self):
      if len(self.a) == 0:
         return None
      return_value = self.a.pop()
      return return_value
    
   def print_stack(self):
      index = self.top
      for line in self.a:
         print line

'''
    get_token function parses the attribute string from a given index and returns the valid token 
    from the string. 
'''
def get_token(string, index):

   i = index 
   token = ''
   new_index = -1
   while i < len(string):
      if string[i] == '"':
         j = i + 1
         while (j < len(string)) and (string[j] != '"'):
            token += string[j]
            j = j + 1
         new_index = j + 1
         i = i + 1
         break;
      elif string[i] == ' ' or string[i] == ')' or string[i] == '(':
         i = i + 1
         continue
      elif (string[i] in ('&', '|', '=')) and (string[i + 1] == string[i]):
         token = string[i] + string[i + 1]
         new_index = i + 2
         break
      elif (string[i] == '!'):
         if (((i + 1) < len(string))  and (string[i + 1] == '=')) :
            token = string[i] + string[i + 1]
            new_index = i + 2
            break
         else:
            token = string[i]
            new_index = i + 1
            break
      else:
         j = i
         while (j < len(string)) and (string[j] not in ('(', ')', '=', '!', '&', ' ', '"')):
            token += string[j]
            j = j + 1
         new_index = j
         i = i + 1
         break;
   
   return (token, new_index)

class BinaryTree():
   
   def __init__(self):
      self.left = None
      self.right = None
      self.rootid = None
   
   def getLeftChild(self):
      return self.left
   def getRightChild(self):
      return self.right
   def setNodeValue(self, value):
      self.rootid = value
   def getNodeValue(self):
      return self.rootid		
   def insertRight(self, newNode):
      if self.right == None:
         self.right = newNode
      else:
         tree = newNode
         tree.right = self.right
         self.right = tree

   def insertLeft(self, newNode):
      if self.left == None:
         self.left = newNode
      else:
         tree = newNode
         self.left = tree
         tree.left = self.left		

'''
    prints the inorder traversal of a tree
	parameters:  
	tree : node of a tree.
'''
def print_tree(tree):
   if tree != None:
      root = tree
      print_tree(tree.getLeftChild())
      print(tree.getNodeValue())
      par = None
      par = get_parent(tree, tree.getNodeValue())
      # if par != None:
         # print "parent of ",tree.getNodeValue()," is :",par.getNodeValue()
      print_tree(tree.getRightChild())
   return 

def get_parent(node, rootid):
   
   if node != None:
      if (node.left != None) and ((node.left).rootid == rootid):
         return node
      elif (node.right != None) and ((node.right).rootid == rootid):
         return node
      get_parent(node.left, rootid)
      get_parent(node.right, rootid)
      
   return None


def precedence(operator):
   if operator == '(':
      return 3
   if operator in ('&&', '||', '!'):
      return 2
   else:
      return 1

def copy_tree(root):
   copyNode = None
   if root != None:
      copyNode = BinaryTree()
      copyNode.rootid = root.rootid
      copyNode.left = copy_tree(root.left)
      copyNode.right = copy_tree(root.right)
   
   return copyNode

def search_tree(parent, node, data):
   searchNode = BinaryTree()
   searchParentNode = BinaryTree()
   if node != None:
      if str(node.rootid) == str(data):
         return (parent, node)
   if node == None:
      return (None, None)    
   
   (searchParentNode, searchNode) = search_tree(node, node.left, data)
   if searchNode != None:
      return (searchParentNode, searchNode)
   (searchParentNode, searchNode) = search_tree(node, node.right, data)
   if searchNode != None:
      return (searchParentNode, searchNode)
   return (None, None)  

def get_string(token):
   if (token in ['true', 'True', '1', 'TRUE']):
      return 'true'
   elif (token in ['false', 'False', '0', 'FALSE']):
      return 'false'
   else:
      return str(token)

'''
    performs == operation on right node's value and attribute value.Left node will contain the attribute.
	parameters:  
	rNodeValue : Left operand 
	attrValue: File attribute's value   
'''          			
def operator_equalequal(rNodeValue, attrValue):
   rNodeValue = get_string(rNodeValue)
   attrValue = get_string(attrValue)
   if str(rNodeValue) == str(attrValue):
      return True
   if str(rNodeValue) not in ['&&', '||', '!=', '!', '==']:
      return False
   return None

'''
    performs != operation on right node's value and attribute value.Left node will contain the attribute.
	parameters:  
	rNodeValue : Left operand 
	attrValue: File attribute's value   
'''  	 
def operatore_notequal(rNodeValue, attrValue):
   rNodeValue = get_string(rNodeValue)
   attrValue = get_string(attrValue)	
   if rNodeValue != str(attrValue):
      return True
   if rNodeValue not in ['&&', '||', '!=', '!']:
      return False
   return None	 

'''
    Performs the operation parent.rootid on right node of a parent and attribute value.
	parameters:  
	rNodeValue : Left operand 
	attrValue: File attribute's value  
''' 	 
def evaluate(parent, rNodeValue, attrValue):
   ret_value = None
   rNodeValue = get_string(str(rNodeValue))
   attrValue = get_string(str(attrValue))
   if parent.rootid == '==':
      ret_value = operator_equalequal(rNodeValue, attrValue)
      if ret_value == None:
         return 
      if ret_value == True:
         parent.rootid = True
      elif ret_value == False:
         parent.rootid = False
      parent.left = None
      parent.right = None
   if parent.rootid == '!=':
      ret_value = operatore_notequal(rNodeValue, attrValue)
      if ret_value == None:
         return 
      if ret_value == True:
         parent.rootid = True
      elif ret_value == False:
         parent.rootid = False
      parent.left = None
      parent.right = None
   elif parent.rootid == '&&':
      if ((rNodeValue == 'true') and (attrValue == 'true')):
         parent.rootid = True
      elif str(rNodeValue) not in ['&&', '!=', '||', '==', '!']:
         parent.rootid = False
      else:
         return
      parent.left = None
      parent.right = None
   elif parent.rootid == '||':
      if ((rNodeValue == 'true') or (attrValue == 'true')):
         parent.rootid = True
      elif parent.right.rootid not in ['&&', '!=', '||', '==', '!']:
         parent.rootid = False
      else:
         return
      parent.left = None
      parent.right = None      
   elif parent.rootid == '!':
      rNodeValue = get_string(str(parent.right.rootid))
      if ((rNodeValue == 'true') and (attrValue == 'true')):
         parent.rootid = False
      elif rNodeValue == 'false':
         parent.rootid = True
      else:
         return
      parent.left = None
      parent.right = None     		
      
   return ret_value
'''
    This function evaluates each leaf node after replacing the attributes with true or false using replace_attribute function.
	parameters:  
	node:node in a attribute tree  
''' 
def final_evaluate(node):
   if node == None:
      return
   if (((node.left != None) and node.left.rootid not in ['&&', '||', '==', '!=', '!']))and ((node.right != None) and  node.right.rootid not in ['&&', '||', '==', '!=', '!']):
   # if (( (node.left != None) and node.left.rootid  in ['true','True','1','TRUE','false','False','0','FALSE']) )and ((node.right != None) and  (node.right.rootid  in ['true','True','1','TRUE','false','False','0','FALSE'])):
      parent = BinaryTree()
      evaluate(node, node.left.rootid, node.right.rootid)
   elif ((node.rootid == '!') and  (node.left == None)and ((node.right != None) and  (node.right.rootid not in ['&&', '||', '==', '!=', '!']))):
   # if (( (node.left != None) and node.left.rootid  in ['true','True','1','TRUE','false','False','0','FALSE']) )and ((node.right != None) and  (node.right.rootid  in ['true','True','1','TRUE','false','False','0','FALSE'])):
      parent = BinaryTree()
      evaluate(node, None, node.right.rootid)
   final_evaluate(node.left)
   final_evaluate(node.right)
   

''' Description: Check each leaf node's value in a file element attributes.
    If the leaf node is an attribute in file element's attrbutes list then replaces the parent node value after performing the operation (operator present in parent's node)
    on left node value and file attribute's value.	
    parameters:
    node - Leaf node in an attribute tree
    parent - parent of  node 
	file - file element 
    Initial values of node and parent are root of a tree.
'''
def replace_attribute(parent, node, file):
   
   if node != None:
      if (node.left == None) and (node.right == None):
         if node.rootid in file.attrib:
            if parent == None:
               print "Tree is not good"
               return False
            elif parent.left != None and parent.right != None:	
               if parent.right.rootid == "Any":
                  if (file.attrib[node.rootid] not in ['false', 'False', 'FALSE', 0, None]):
                     parent.rootid = True
                  else:
                     parent.rootid = False
                  parent.right = None
                  parent.left = None
               else:
                  evaluate(parent, parent.right.rootid, file.attrib[node.rootid])
            elif (parent.rootid == '!') and (parent.right != None) :
               evaluate(parent, parent.right.rootid, file.attrib[node.rootid])
         
         else: 
            parent.rootid = False
            parent.left = parent.right = None
      replace_attribute(node, node.left, file)
      replace_attribute(node, node.right, file)
      

def number_of_nodes(root):
   if root == None:
      return 0
   if root.left == None and root.right == None:
      return 1
   
   return (1 + number_of_nodes(root.left) + number_of_nodes(root.right))
       
def insert_node(token, stack2):
   if token in ('&&', '||', '==', '!='):
      n1 = stack2.pop()
      n2 = stack2.pop()
      newNode = BinaryTree()
      newNode.setNodeValue(token)
      newNode.insertLeft(n2)
      newNode.insertRight(n1)
      stack2.push(newNode)	  
   elif token == '!':
      n1 = stack2.pop()
      newNode = BinaryTree()
      newNode.setNodeValue(token)
      newNode.insertRight(n1)
      stack2.push(newNode)               
   return stack2

'''Description: Evaluate the file element's attribute with the attribute string tree.
   parameters :
   root - root node of a attrbute string
   file - file element 
'''
def eval(root, file):
   # print "Eval:"
   root2 = copy_tree(root)
   replace_attribute(root2, root2, file)
   len = number_of_nodes(root2)
   while 	len != 1:
      final_evaluate(root2)
      len = number_of_nodes(root2)
   return root2.rootid

'''Description: parse attribute string and construct a binary tree.
   parameters :
   attr - file element's attribute.
'''
def parse_attr_string(attr):
   string = attr
   string = '(' + string + ')'
   # print "Infix String:",string
   postfixString = ''
   st = stack()
   stack2 = stack()
   i = 0
   root = None
   while i < (len(string)):
      if string[i] == '(' :
         st.push('(')
         i = i + 1
         continue
      elif string[i] == ')':
         popedItem = st.pop()
         count_of_tokens = 0
         while (popedItem != '(') and (popedItem != None):
            # print "44 ",temp
            stack2 = insert_node(popedItem, stack2)                
            popedItem = st.pop()
            count_of_tokens += 1
            if popedItem == None:
               break
         if count_of_tokens == 0:
            n2 = BinaryTree()
            n2.setNodeValue("Any")
            n1 = stack2.pop()
            newNode = BinaryTree()
            newNode.setNodeValue("==")
            newNode.insertLeft(n1)
            newNode.insertRight(n2)
            stack2.push(newNode)
         i = i + 1
      elif string[i] == ' ':
         i = i + 1
         continue
      else:
         (token, newIndex) = get_token(string, i)
         if token in ('&&', '||', '!'):
            while  (len(st.a) > 1) and (precedence(st.a[len(st.a) - 1]) < precedence(token)):
               popItem = st.pop() 
               stack2 = insert_node(popItem, stack2)	
            st.push(token)
            i = newIndex          
         elif token in ('==', '!='):
            st.push(token)
         else:
            # postfixString += token
            newNode = BinaryTree()
            newNode.setNodeValue(token)
            stack2.push(newNode)
            
         i = newIndex 
         if newIndex == -1:
            break
   
   while len(st.a) > 0:
      token = st.pop()
      if token != '(': 
         if token in ('&&', '||', '==', '!=', '!'):
            stack2 = insert_node(token, stack2)	
         else:
            newNode = BinaryTree()
            newNode.setNodeValue(token)
            stack2.push(newNode)
   root = stack2.pop()
   st = stack()
   # print"\n\nInorder Traversal of a binary tree:"
   # print_tree(root)
   return root
