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

bool isEqual(list <struct atom> x, list <struct atom> y) {
  list <struct atom>::iterator it1 = x.begin();
  list <struct atom>::iterator it2 = y.begin();
  while(it1 != x.end() && it2 != x.end() && (*it1).val == (*it2).val) {
    it1++;
    it2++;
  }
  if (it1 == x.end() && it2 == y.end()) {
    return true;
  } else {
    return false;
  }
}

map<struct atom, struct atom> unify(struct atom x, struct atom y, map<struct atom, struct atom> &theta, bool &failure);

map<struct atom, struct atom> unifyVar(struct atom var, struct atom x,
    map<struct atom, struct atom> &theta, bool failure) {

  if (theta.find(var) != theta.end()) {
    // if {var/val} E theta then return UNIFY(val, x, theta)
    return unify(theta[var], x, theta, failure);
  } else if (theta.find(x) != theta.end()) {
    // else if {x/val} E theta then return UNIFY(var, val, theta)
    return unify(var, theta[x], theta, failure);
  } else {
    // else return add {var/x} to theta
    theta[var] = x;
    return theta;
  }
}


map<struct atom, struct atom> unify(struct atom x, struct atom y, map<struct atom, struct atom> &theta, bool &failure) {
    if (failure) {
      return theta; 
    } else if (x.type == "VARIABLE") {
      return unifyVar(x, y, theta, failure);
    } else if (y.type == "VARIABLE") {
      return unifyVar(y, x, theta, failure);
    } else if (x.val == y.val) {
      return theta;
    } else {
      failure = true;
      return theta;
    }
}


map<struct atom, struct atom> unify(list <struct atom> x, list <struct atom> y, map<struct atom, struct atom> &theta, bool &failure) {
  cout << "asdasdsdasdas" << endl;
  if (failure) {
    return theta;
  } else if (x.size() != y.size()) {
    failure = true;
    return theta;
  } else if (x.size() == 0 && y.size() == 0) {
    return theta;
  } else if (isEqual(x,y)) {
    cout << "asdsad";
    return theta;
  } else if (x.size() == 1 && y.size() == 1) {
    return unify(x.front(), y.front(), theta, failure);
  } else {
    struct atom st1 = x.front();
    struct atom st2 = y.front();
    x.pop_front();
    y.pop_front();
    theta = unify(st1, st2, theta, failure); 
    return unify(x, y, theta, failure);
  }
}


list <struct atom> atomize(string s) {
  vector <string> s_split = split(s, '(');
  s_split = split(s_split[1],')');
  s_split = split(s_split[0],',');
  list <struct atom> atoms;
  struct atom a; 
  for (int i = 0; i < s_split.size(); ++i)
  {
    if (s_split[i].length() == 1) {
      a.type = "VARIABLE";
    } else {
      a.type = "LITERAL";
    }
    a.val = s_split[i];
    atoms.push_back(a);
  }

  return atoms;
}

int main(int argc, char const *argv[])
{
    int in_size;
    vector<string> input;
    // for A(X) => B(Y) stores A(x) as 0 and B(Y) as 1
    vector< vector <string> > storeIntermediate;
    vector<string> postfix;
    string in;
    cin>>in_size;
    getline(cin,in);
    for (int i = 0; i <= in_size; i++) {
        if (i == 0) {
            continue;
        }
        getline(cin,in);
        input.push_back(in);
    }
    //cout << input.size() << endl;
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
    	cout << "asda" << endl;
        cout << postfix[i] << endl;
        //inorder(root);
        et* root_cnf = convertToCNF(root);
        cout << "converted to cnf" << endl;
        string s;
        inorder(root_cnf, s);
        s = convertToOriginal(s, storeIntermediate[i]);
        cout << s << endl;
        vector <string> temp = split(s,'&');
        map <string, vector <int> > table;
        for (int j = 0; j < temp.size(); ++j)
        {
        	knowledgeBase.push_back(temp[j]);
        }
    }
    map <string,struct s_table> table;
    cout << "+++++++++++++++++++ knowledgeBase -----------------------" << endl;
    for (int i = 0; i < knowledgeBase.size(); ++i) {
        doTableBasedIndexing(table, knowledgeBase[i], i);
    	cout << knowledgeBase[i] << endl;
    }
    cout << "table based index ============================ " << endl;
    for (map <string, struct s_table>::iterator it = table.begin(); it != table.end();it++) {
    	cout << "predicate == " << it->first << endl;
    	for (int i = 0; i < table[it->first].positive.size(); ++i)
    	{
    		cout << table[it->first].positive[i] << " ";
    	}
    	cout << endl << "negative" << endl;
    	for (int i = 0; i < table[it->first].negative.size(); ++i)
    	{
    		cout << table[it->first].negative[i] << " ";
    	}
    	cout << endl;
    }

    return 0;
}