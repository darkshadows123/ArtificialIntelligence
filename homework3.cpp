#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <climits>
#include <stack>
#include <map>
#include <list>
using namespace std;

struct et
{
    char value;
    et* left, *right;
};

struct s_table
{
    vector<int> positive;
    vector<int> negative;
};


struct atom {
  string type;
  string val;
};
bool operator< ( atom a, atom b ) { return std::make_pair(a.type,a.val) < std::make_pair(b.type,b.val) ; }
     
map <string,string> argType;
map <string, bool> visited;
map <string, bool> hashMapKB;
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

// A utility function to check if 'c'
// is an operator
bool isOperator(char c)
{
    if (c == '~' || c == '|' ||
            c == '#' || c == '&' ||
            c == '$')
        return true;
    return false;
}
     
// Utility function to do inorder traversal
void inorder(et *t, string &s)
{
    if(t)
    {
        //printf("( ");
        inorder(t->left, s);
        s = s + t->value;
        //printf("%c", t->value);
        inorder(t->right, s);
        //printf(")");
    }
}
 
// A utility function to create a new node
et* newNode(char v)
{
    et *temp = new et;
    temp->left = temp->right = NULL;
    temp->value = v;
    return temp;
};

// A utility function to create a new node
et* newNode(char v, et* left, et* right)
{
    et *temp = new et;
    temp->left = left;
    temp->right = right;
    temp->value = v;
    return temp;
};

// A utility function to create a new node
et* newNode(et* node)
{
    et *temp = new et;
    temp->left = node->left == NULL ? NULL : newNode(node->left);
    temp->right = node->right == NULL ? NULL : newNode(node->right);
    temp->value = node->value;
    return temp;
};
// Returns root of constructed tree for given
// postfix expression
et* constructTree(string postfix)
{
    stack<et *> st;
    et *t, *t1, *t2;
 
    // Traverse through every character of
    // input expression
    for (int i=0; i<postfix.length(); i++)
    {
        // If operand, simply push into stack
        if (!isOperator(postfix[i]))
        {
            t = newNode(postfix[i]);
            st.push(t);
        }
        else // operator
        {
            t = newNode(postfix[i]);
            if (postfix[i] == '~') {

            // Pop two top nodes
            t1 = st.top(); // Store top
            st.pop();
 
            //  make them children
            t->right = t1;
            } else {
            // Pop two top nodes
            t1 = st.top(); // Store top
            st.pop();      // Remove top
            t2 = st.top();
            st.pop();
 
            //  make them children
            t->right = t1;
            t->left = t2;
            }
            // Add this subexpression to stack
            st.push(t);
        }
    }
 
    //  only element will be root of expression
    // tree
    t = st.top();
    st.pop();
 
    return t;
}

bool IsLeaf(et* node)
{
    return node->left == NULL && node->right == NULL;
}

bool IsAtomic(et* node)
{
    return (IsLeaf(node) || (node->value == '~' && IsLeaf(node->right)));
}

et* DriveInNegation(et* node)
{
    if (node->value == '~')
    {
        //NOTE: there may be many consecutive NOT, for example: ￢￢￢￢A
        if (node->right->value == '~')
        {
            return DriveInNegation(node->right->right);
        }
        else
        {
            return newNode(node->right);
        }
    }
    else if (node->value == '&')
    {
        return newNode('|', DriveInNegation(node->left), DriveInNegation(node->right));
    }
    else if (node->value == '|')
    {
        return newNode('&', DriveInNegation(node->left), DriveInNegation(node->right));
    }
    else if (node->value == '#')
    {
        return newNode('&', node->left, DriveInNegation(node->right));
    }
    else
    {
        //cout << "asdd" << endl;
        return newNode('~', NULL, node);
    }
}

et* convertToCNF(et * root) {
    // cout << "root value ==== " << root->value << endl; 
    // if (root->left)
    // cout << "root left value ==== " << root->left->value << endl;
    // if (root->right)
    // cout << "root right value ==== " << root->right->value << endl;
    if (IsLeaf(root))
        return newNode(root->value);
    if (root->value == '~')
    {
        if (IsLeaf(root->right))
        // ￢A
        {
            // cout << "~A" << endl;
            return newNode(root);
        }
        else
        // ￢(...)
        {
            // cout << "~(...)" << endl;
            et* expr = DriveInNegation(root->right);
            return convertToCNF(expr);
        }
    }

    // convert children first
    et *cnfLeft = NULL, *cnfRight = NULL;
    if (root->left  != NULL) {
        cnfLeft  = convertToCNF(root->left);
    }
    if (root->right != NULL) {
        cnfRight = convertToCNF(root->right);
    }
    if (root->value == '&') {
        return newNode('&',cnfLeft,cnfRight);
    }
    if (root->value == '#')
    {

    // cout << "root value ==== " << root->value << endl; 
    // if (root->left)
    // cout << "root left value ==== " << root->left->value << endl;
    // if (root->right)
    // cout << "root right value ==== " << root->right->value << endl;
        // cout << "A=>B" << endl;
        et* impLeft = DriveInNegation(root->left);
        et* impRight = root->right;
        et* temp = newNode('|', impLeft, impRight);
        return convertToCNF(temp);
    }
    if (root->value == '|') {
        if (( cnfLeft == NULL || IsAtomic(cnfLeft) || cnfLeft->value == '|')
            && (cnfRight == NULL || IsAtomic(cnfRight) || cnfRight->value == '|'))
        //   +
        // +   +
        {
            return newNode('|',cnfLeft,cnfRight);
        }
        else if ((cnfLeft != NULL && cnfLeft->value == '&')
                && (cnfRight == NULL || IsAtomic(cnfRight) || cnfRight->value == '|'))
        //   +
        // *   +
        {
            et* newLeft = newNode('|', cnfLeft->left, cnfRight);
            et* newRight = newNode('|', cnfLeft->right, cnfRight);

            return newNode('&', convertToCNF(newLeft), convertToCNF(newRight));
        }
        else if ((cnfRight != NULL && cnfRight->value == '&')
                && (cnfLeft == NULL || IsAtomic(cnfLeft) || cnfLeft->value == '|'))
        //   +
        // +   *
        {
            et* newLeft = newNode('|', cnfLeft, cnfRight->right);
            et* newRight = newNode('|', cnfLeft, cnfRight->left);

            return newNode('&', convertToCNF(newLeft), convertToCNF(newRight));
        }
        else if ((cnfLeft != NULL && cnfLeft->value == '&')
                && (cnfRight != NULL && cnfRight->value == '&'))
        //   +
        // *   *
        {
            et* newLeft = newNode('&',
                newNode('|', cnfLeft->left, cnfRight->left),
                newNode('|', cnfLeft->right, cnfRight->left));

            et* newRight = newNode('&',
                newNode('|', cnfLeft->left, cnfRight->right),
                newNode('|', cnfLeft->right, cnfRight->right));

            return newNode('&', convertToCNF(newLeft), convertToCNF(newRight));
        }
    }
    cout << "error!!!!!!!!!!!!!" << endl;
    return NULL;

}

// Function to verify whether a character is english letter or numeric digit. 
// We are assuming in this solution that operand will be a single character
bool IsOperand(char C) 
{
    if(C >= '0' && C <= '9') return true;
    if(C >= 'a' && C <= 'z') return true;
    if(C >= 'A' && C <= 'Z') return true;
    return false;
}

// Function to verify whether a character is operator symbol or not. 
bool IsOperator(char C)
{
    if(C == '#' || C == '&' || C == '|' || C == '~' || C== '$')
        return true;

    return false;
}

// Function to verify whether an operator is right associative or not. 
int IsRightAssociative(char op)
{
    if(op == '$') return true;
    return false;
}

// Function to get weight of an operator. An operator with higher weight will have higher precedence. 
int GetOperatorWeight(char op)
{
    int weight = -1; 
    switch(op)
    {
    case '#':
        weight = 1;
    case '&':
    case '|':
        weight = 2;
    case '~':
        weight = 3;
    case '$':
        weight = 4;
    }
    return weight;
}

// Function to perform an operation and return output. 
int HasHigherPrecedence(char op1, char op2)
{
    int op1Weight = GetOperatorWeight(op1);
    int op2Weight = GetOperatorWeight(op2);

    // If operators have equal precedence, return true if they are left associative. 
    // return false, if right associative. 
    // if operator is left-associative, left one should be given priority. 
    if(op1Weight == op2Weight)
    {
        if(IsRightAssociative(op1)) return false;
        else return true;
    }
    return op1Weight > op2Weight ?  true: false;
}


// Function to evaluate Postfix expression and return output
string InfixToPostfix(string expression)
{
    // Declaring a Stack from Standard template library in C++. 
    stack<char> S;
    string postfix = ""; // Initialize postfix as empty string.
    for(int i = 0;i< expression.length();i++) {

        // Scanning each character from left. 
        // If character is a delimitter, move on. 
        if(expression[i] == ' ' || expression[i] == ',') continue; 

        // If character is operator, pop two elements from stack, perform operation and push the result back. 
        else if(IsOperator(expression[i])) 
        {
            while(!S.empty() && S.top() != '(' && HasHigherPrecedence(S.top(),expression[i]))
            {
                postfix+= S.top();
                S.pop();
            }
            S.push(expression[i]);
        }
        // Else if character is an operand
        else if(IsOperand(expression[i]))
        {
            postfix +=expression[i];
        }

        else if (expression[i] == '(') 
        {
            S.push(expression[i]);
        }

        else if(expression[i] == ')') 
        {
            while(!S.empty() && S.top() !=  '(') {
                postfix += S.top();
                S.pop();
            }
            S.pop();
        }
    }

    while(!S.empty()) {
        postfix += S.top();
        S.pop();
    }

    return postfix;
}


string convertToOriginal(string converted, vector<string> helper) {
    string ans = "";
    for (int i = 0; i < converted.length(); ++i)
    {
        if (!IsOperator(converted[i])) {
            ans = ans + helper[converted[i] - '0'];
        } else {
            ans = ans + converted[i];
        }
    }
    return ans;
}

string convertToSimple(string original, vector < vector<string> >& storeIntermediate) {
    string converted = "";
    original.erase(remove(original.begin(), original.end(), ' '), original.end());
    int i = 0;
    int len = original.length();
    int count = 0;
    vector<string> v;
    while (i < len) {
        if ('A' <= original[i] && original[i] <= 'Z') {
            string temp = "";
            while(i < len) {
                temp = temp + original[i];
                if(original[i] == ')')
                    break;
                i++;
            }
            i++;
            v.push_back(temp);
            stringstream sstm;
            sstm << converted << count;
            converted = sstm.str();
            count++;
            temp.clear();
        } else if (original[i] == '=' && original[i + 1] == '>') {
                converted += '#';
            i = i + 2;
        } else {
            converted += original[i];
            i++;
        }
    }
    storeIntermediate.push_back(v);
    cout << converted << endl;
    return converted;
}




void doTableBasedIndexing( map < string, struct s_table> &table, string s, int ind) {
    std::vector<string> orSplit = split(s, '|');
    for (int i = 0; i < orSplit.size(); ++i)
    {
        bool negation = false;
        string predicate = "";
        for (int j = 0; j < orSplit[i].length(); ++j)
        {
            if (orSplit[i][j] == '~') {
                negation = true;
            }
            if ('A' <= orSplit[i][j] && orSplit[i][j] <= 'Z') {
                while(orSplit[i][j] != '(') {
                    predicate = predicate + orSplit[i][j];
                    j++;
                }
                if (negation)
                    table[predicate].negative.push_back(ind);
                else
                    table[predicate].positive.push_back(ind);
                break;
            }
        }
    }
}

map<struct atom, struct atom> unify(list<struct atom> atoms, list <struct atom> atoms1, bool &failure) {
  map <struct atom, struct atom> theta;
  list <struct atom>:: iterator it1 = atoms.begin();
  list <struct atom>:: iterator it2 = atoms1.begin();
  while (!atoms.empty() && !atoms1.empty()) {
    struct atom x = *it1;
    struct atom y = *it2;
    it1++;
    it2++;
    atoms.pop_front();
    atoms1.pop_front();
    if (x.val != y.val) {
        if (x.type == "VARIABLE") {
          list <struct atom>:: iterator temp_it1 = atoms.begin();
          list <struct atom>:: iterator temp_it2 = atoms1.begin();
          while(temp_it1 != atoms.end()) {
            if ((*temp_it1).val == x.val) {
              (*temp_it1).val = y.val;
              (*temp_it1).type = y.type;
            }
            temp_it1++;
          }
          while(temp_it2 != atoms1.end()) {
            if ((*temp_it2).val == x.val) {
              (*temp_it2).val = y.val;
              (*temp_it2).type = y.type;
            }
            temp_it2++;
          }
          map<struct atom, struct atom>::iterator theta_it = theta.begin();
          while(theta_it != theta.end()) {
            if ((*theta_it).second.val == x.val) {
              (*theta_it).second.val = y.val;
              (*theta_it).second.type = y.type;
             }
             theta_it++;
          }
          theta[x] = y;
        } else if (y.type == "VARIABLE") {
          list <struct atom>:: iterator temp_it1 = atoms.begin();
          list <struct atom>:: iterator temp_it2 = atoms1.begin();
          while(temp_it1 != atoms.end()) {
            if ((*temp_it1).val == y.val) {
              (*temp_it1).val = x.val;
              (*temp_it1).type = x.type;
            }
            temp_it1++;
          }
          while(temp_it2 != atoms1.end()) {
            if ((*temp_it2).val == y.val) {
              (*temp_it2).val = x.val;
              (*temp_it2).type = x.type;
            }
            temp_it2++;
          }
          map<struct atom, struct atom>::iterator theta_it = theta.begin();
          while(theta_it != theta.end()) {
            if ((*theta_it).second.val == y.val) {
              (*theta_it).second.val = x.val;
              (*theta_it).second.type = x.type;
             }
             theta_it++;
          }
          theta[y] = x;
        } else {
            theta.clear();
            failure = true;
            return theta;
        }
    }
  }

  return theta;
}



list <struct atom> atomize(string s) {
  vector <string> s_split = split(s, '(');
  s_split = split(s_split[1],')');
  s_split = split(s_split[0],',');
  list <struct atom> atoms;
  struct atom a; 
  for (int i = 0; i < s_split.size(); ++i)
  {
    a.type = argType[s_split[i]];
    a.val = s_split[i];
    atoms.push_back(a);
  }

  return atoms;
}


void standarizeVariables(string &clause, int ind) {
  vector <string> predicatesWithArgs = split(clause,'|');
  clause = "";
  for (int i = 0; i < predicatesWithArgs.size(); ++i)
  {
      string s = predicatesWithArgs[i];
      vector <string> s_split = split(s, '(');
      s = "";
      s = s + s_split[0] + '(';
      s_split = split(s_split[1],')');
      s_split = split(s_split[0], ',');
      stringstream ss;
      string arg;
      for (int j = 0; j < s_split.size(); ++j) {
        if ('a' <= s_split[j][0] && s_split[j][0] <= 'z') {
            ss << ind;
            arg = s_split[j] + ss.str();
            argType[arg] = "VARIABLE";
            ss.str("");
        } else {
            arg = s_split[j];
            argType[arg] = "LITERAL";
        }
        if ( j == 0) {
            s = s + arg;
        } else {
            s = s + ',' + arg; 
        }
      }
      s = s + ')';

      if (i == 0) {
        clause  = clause + s;
      } else {
        clause = clause + '|' + s;
      }
  }
}


string getPredicate(string s, bool &isPositive) {
    isPositive = true;
    string predicate = "";
    for (int i = 0; i < s.length(); ++i) {
        if (s[i] == '~') {
            isPositive = false;
        }
        if ('A' <= s[i] && s[i] <= 'Z') {
            while(s[i] != '(') {
                predicate = predicate + s[i];
                i++;
            }
            break;
        }
    }

    return predicate;
}

void removeSubstrs(string& s, string& p) { 
  string::size_type n = p.length();
  for (string::size_type i = s.find(p);
      i != string::npos;
      i = s.find(p))
      s.erase(i, n);
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

string unifyAndGenerateNextquery(string clause1, string clause2, map <struct atom, struct atom> theta, string predicateToRemove) {
  map <struct atom, struct atom>::iterator it;
  // cout << "+++++++++++theta----------------" << endl;
  for (it = theta.begin(); it != theta.end(); ++it) {
    // cout << (*it).first.val << " " << (*it).second.val << endl;
    replaceAll(clause1, (*it).first.val, (*it).second.val);
    replaceAll(clause2, (*it).first.val, (*it).second.val);
    replaceAll(predicateToRemove, (*it).first.val, (*it).second.val);
  }
  // cout << "predicateToRemove = " << predicateToRemove << endl;
  string nextQuery =  clause1 + '|' + clause2;
  if (predicateToRemove[0] == '~') {
    removeSubstrs(nextQuery, predicateToRemove);
    string temp = predicateToRemove.substr(1);
    removeSubstrs(nextQuery, temp);
  } else {
    string temp = '~' + predicateToRemove;
    removeSubstrs(nextQuery, temp);
    removeSubstrs(nextQuery, predicateToRemove);
  }
  // cout << "before cleaning nextQuery = " << nextQuery << endl;
  // cout << "clause1 =  " << clause1 << " clause2 = " << clause2 << endl;
  vector <string> nextQuery_split = split(nextQuery,'|');
  nextQuery = "";
  for (int i = 0; i < nextQuery_split.size(); ++i) {
    if (nextQuery_split[i].empty() || nextQuery_split[i] == "")
        continue;
    if (nextQuery == "") {
        nextQuery += nextQuery_split[i];
    } else {
        nextQuery = nextQuery + "|" + nextQuery_split[i];
    }
  }

  return nextQuery;
}

int maxLen;
void dfs(string query, vector<string> knowledgeBase, map<string, struct s_table> table, bool &found, int count) {
    if (found == true)
        return;
    cout << endl;
    cout << "-------------------------------dfs number << " << count << " starts here---------------------------------------------------------" << endl;
    cout << "query === " << query << endl;
    string clause1 = query;
    vector <string> cl1_split = split(clause1,'|');
    // if (cl1_split.size() > maxLen)
    //     return;
    for (int i = 0; i < cl1_split.size(); ++i)
    {
        bool isCl1Positive = true;
        string cl1_predicate = getPredicate(cl1_split[i], isCl1Positive);
        // cout << "cl1_predicate = " << cl1_predicate << " isCl1Positive = " << isCl1Positive << endl;
        if (isCl1Positive) {
            for (int j = 0 ; j < table[cl1_predicate].negative.size() ;j++) {
                if (found == true) {
                    return;
                }
                string clause2 = knowledgeBase[table[cl1_predicate].negative[j]];
                vector <string> cl2_split = split(clause2, '|');
                for (int k = 0 ; k < cl2_split.size();k++) {
                    if (found == true) {
                        return;
                    }
                    bool isCl2Positive = true;
                    string cl2_predicate = getPredicate(cl2_split[k], isCl2Positive);
                    // check whether two predicates can be unified or not
                    if (cl2_predicate == cl1_predicate && isCl1Positive != isCl2Positive) {
                        cout << "in positive unifying " << clause1 << " " << clause2 << endl;
                        
                        if (visited[clause1 + clause2]) {
                            continue;
                        }
                        bool failure = false;
                        map <struct atom, struct atom> theta = unify(atomize(cl1_split[i]), atomize(cl2_split[k]), failure);
                        if (theta.empty() && failure) {
                            cout << "asda" << endl;
                            continue;
                        }

                        string nextQuery = unifyAndGenerateNextquery(clause1, clause2, theta, cl2_split[k]);
                        cout << "nextQuery = " << nextQuery << endl;  
                        if (nextQuery == "") {
                            found = true;
                            return;
                        } else {

                            visited[clause1 + clause2] = true;
                            dfs(nextQuery, knowledgeBase, table,found, count + 1);
                            visited[clause1 + clause2] = false;
                        } 
                    }
                }
            }
        } else {
            for (int j = 0 ; j < table[cl1_predicate].positive.size() ;j++) {
                if (found == true) {
                    return;
                }
                // cout << "table index  = " << table[cl1_predicate].positive[j] << endl;
                string clause2 = knowledgeBase[table[cl1_predicate].positive[j]];
                vector <string> cl2_split = split(clause2, '|');
                for (int k = 0 ; k < cl2_split.size();k++) {
                    if (found == true) {
                        return;
                    }
                    bool isCl2Positive = true;
                    string cl2_predicate = getPredicate(cl2_split[k], isCl2Positive);
                    // cout << "checking cl2_predicate = " << cl2_predicate << endl;
                    // check whether two predicates can be unified or not
                    if (cl2_predicate == cl1_predicate && isCl1Positive != isCl2Positive) {
                        cout << "in negative unifying " << clause1 << " " << clause2 << endl;
                        bool failure = false;
                        if (visited[clause1 + clause2]) {
                            continue;
                        }
                        map <struct atom, struct atom> theta = unify(atomize(cl1_split[i]), atomize(cl2_split[k]), failure);
                        if (theta.empty() && failure) {
                            continue;
                        }
                        // if (!validTheta(theta)) {
                        //     continue;
                        // }
                        string nextQuery = unifyAndGenerateNextquery(clause1, clause2, theta, cl2_split[k]);
                        cout << "nextQuery = " << nextQuery << endl;
                        //return;  
                        if (nextQuery == "") {
                            found = true;
                            return;
                        } else {
                            visited[clause1 + clause2] = true;
                            dfs(nextQuery, knowledgeBase, table,found, count + 1);
                            visited[clause1 + clause2] = false;
                        } 
                    }
                }
            }
        }
    }
    cout << "+++++++++++++++++++++++++++fnished dfs count = " << count << "++++++++++++++" << endl;
}

bool validTheta(map <struct atom, struct atom> theta) {
  map <struct atom, struct atom>::iterator it;
  bool ans = true;
  for (it = theta.begin(); it != theta.end(); ++it) {
    if ((*it).second.type == "VARIABLE")
        return false;
  }

  return true;
}
bool isFact(vector <string> cl_split)  {
    if (cl_split.size() > 1)
        return false;
    vector <string> s_split = split(cl_split[0], '(');
    s_split = split(s_split[1],')');
    s_split = split(s_split[0],',');
    for (int i = 0; i < s_split.size(); ++i)
    {
        if (argType[s_split[i]] == "VARIABLE")
            return false;
    }
  return true;
}
void getAllClauses(string clause1, string clause2, vector <string>& clauses) {
    vector <string> cl1_split = split(clause1,'|');
    vector <string> cl2_split = split(clause2,'|');
    if (isFact(cl1_split) || isFact(cl2_split)) {
        for (int i = 0; i < cl1_split.size(); ++i) {
            bool isCl1Positive = true;
            string cl1_predicate = getPredicate(cl1_split[i], isCl1Positive);
            for (int j = 0; j < cl2_split.size(); ++j) {
                bool isCl2Positive = true;
                string cl2_predicate = getPredicate(cl2_split[j], isCl2Positive);
                if (cl1_predicate == cl2_predicate && isCl1Positive != isCl2Positive) {
                    bool failure = false;
                    map <struct atom, struct atom> theta = unify(atomize(cl1_split[i]), atomize(cl2_split[j]), failure);
                    if (theta.empty() && failure) {
                        continue;
                    }
                    // if (!validTheta(theta))
                    //     continue;
                    string newClause = unifyAndGenerateNextquery(clause1, clause2, theta, cl2_split[j]);
                    if (newClause == "") {
                        cout << "EMPTY " << clause1 << " " << clause2 <<endl;
                    }
                    vector <string> nc_split = split(newClause, '|');
                    int len = nc_split.size();
                    if (len > maxLen)
                        continue;
                    clauses.push_back(newClause);
                }
            }
        }
    }
}

void resolution (vector <string> knowledgeBase, bool& found) {
    found = false;
    int while_count =  0;
    while (true) {
        cout << "while_count = " << while_count << endl;
        while_count++;
        int newCount = 0;
        vector <string> newClauses;
        for (int i = 0; i < knowledgeBase.size(); ++i) {
            string clause1 = knowledgeBase[i];
            for (int j = i + 1; j < knowledgeBase.size(); ++j) {
                string clause2 = knowledgeBase[j];
                //cout << "unifying " << clause1 << " " << clause2 << endl;
                getAllClauses(clause1, clause2, newClauses);
            }
        }
        for (int k = 0; k < newClauses.size(); ++k) {
            if (newClauses[k] == "") {
                cout << "EMPTY()" << endl;
                found = true;
                return;
            }
            if (hashMapKB.find(newClauses[k]) == hashMapKB.end() ) {
                cout << newClauses[k] << endl;
                hashMapKB[newClauses[k]] = true;
                newCount++;
                knowledgeBase.push_back(newClauses[k]);
            }
        }
        if (newCount == 0) {
            found = false;
            return;
        }
        //cout << newCount << endl;
    }
    // cout << "============================ new kb ==================================" << endl;
    // for (int i = 0; i < knowledgeBase.size(); ++i)
    // {
    //     cout << knowledgeBase[i] << endl;
    // }
}




int main(int argc, char const *argv[])
{
    int in_size;
    vector<string> input;
    // for A(X) => B(Y) stores A(x) as 0 and B(Y) as 1
    vector< vector <string> > storeIntermediate;
    vector<string> postfix;
    vector<string> queries;
    int q_size;
    string in;
    cin >> q_size;
    getline(cin,in);
    for (int i = 0; i <= q_size; ++i)
    {
        if (i == 0) {
            continue;
        }
        getline(cin,in);
        if (in[0] == '~') {
            in = in.substr(1);
        } else {
            in = '~' + in;
        }
        queries.push_back(in);
    }


    cin>>in_size;
    getline(cin,in);
    for (int i = 0; i <= in_size; i++) {
        if (i == 0) {
            continue;
        }
        getline(cin,in);
        input.push_back(in);
    }
    cout << input.size() << endl;
    for (int i = 0; i < in_size; ++i)
        postfix.push_back(InfixToPostfix(convertToSimple(input[i], storeIntermediate)));
    // for (int i = 0; i < in_size; ++i)
    // {
    //     cout << "i = " << i << endl;
    //     for (int j = 0; j < storeIntermediate[i].size(); ++j)
    //     {
    //         cout << storeIntermediate[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    std::vector<string> knowledgeBase;
    for (int i = 0; i < in_size; ++i)
    {
        et* root = constructTree(postfix[i]);
        //cout << "asda" << endl;
        //cout << postfix[i] << endl;
        //inorder(root);
        et* root_cnf = convertToCNF(root);
        //cout << "converted to cnf" << endl;
        string s;
        inorder(root_cnf, s);
        s = convertToOriginal(s, storeIntermediate[i]);
        cout << s << endl;
        vector <string> temp = split(s,'&');
        for (int j = 0; j < temp.size(); ++j)
        {
            knowledgeBase.push_back(temp[j]);
        }
        // avoid memory leak
        delete root;
        delete root_cnf;
    }
    map <string,struct s_table> table;
    maxLen = INT_MIN;
    cout << "+++++++++++++++++++ knowledgeBase -----------------------" << endl;
    for (int i = 0; i < knowledgeBase.size(); ++i) {
        standarizeVariables(knowledgeBase[i], i);
        cout << knowledgeBase[i] << endl;
        vector <string> temp1  = split(knowledgeBase[i], '|');
        //cout << temp1.size() << " " << maxLen << endl;
        int len = temp1.size();
        maxLen  = max(maxLen,len);
        doTableBasedIndexing(table, knowledgeBase[i], i);
        temp1.clear();
    }

    //cout << maxLen << endl;
    //return 0;
        // cout << "table based index ============================ " << endl;
        // for (map <string, struct s_table>::iterator it = table.begin(); it != table.end();it++) {
        //     cout << "predicate == " << it->first << endl;
        //     for (int i = 0; i < table[it->first].positive.size(); ++i)
        //     {
        //         cout << table[it->first].positive[i] << " ";
        //     }
        //     cout << endl << "negative" << endl;
        //     for (int i = 0; i < table[it->first].negative.size(); ++i)
        //     {
        //         cout << table[it->first].negative[i] << " ";
        //     }
        //     cout << endl;
        // }
        // cout << "Type of argument ========================== " << endl;

        // for (map <string,string>::iterator it = argType.begin(); it != argType.end(); ++it)
        // {
        //     cout << it->first << " " << it->second << endl;
        // }
    std::vector<bool> ans;
    for (int i = 0; i < queries.size(); ++i)
    {
        visited.clear();
        knowledgeBase.push_back(queries[i]);
        for (int j = 0; j < knowledgeBase.size(); ++j)
            hashMapKB[knowledgeBase[j]] = true;
        //doTableBasedIndexing(table, knowledgeBase[knowledgeBase.size() - 1], knowledgeBase.size() - 1);
        standarizeVariables(knowledgeBase[knowledgeBase.size() - 1], knowledgeBase.size() - 1);
        bool found = false;
        resolution (knowledgeBase, found);
        //dfs (queries[i], knowledgeBase, table, found, 0);
        ans.push_back(found);
        knowledgeBase.pop_back();
        hashMapKB.clear();
        //break;
    }
    for (int i = 0; i < ans.size(); ++i)
    {
        cout << ans[i] << endl;
    }
    return 0;
}