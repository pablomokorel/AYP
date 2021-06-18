/*Este archivo contiene funciones para manipular y transformar strings
*/

#include <string>
#include "stringUtils.h"
#include "Stack.h"

const string MATH_NEG = "-1"; //GLOBALES
const string MATH_SEPARATORS = "+-*/()"; //+-*/()^
const string MATH_OPT = "+-*/"; //Validar doble operadores
const string MATH_SIMBOLS = "eE"; // eE

bool isBalanced(const string &expr){

    // Creo el stack
    Stack<char> s;
    char c;

    for(unsigned int i = 0; i < expr.length(); i++){

        // Cargo el caracter a parsear
        c = expr[i];

        // Si abro una expresi�n, pusheo el caracter al stack
        if( c == '(' || c == '[' || c == '{' ){
            if (c == '(')
                s.push(')');
            else if(c == '{')
                s.push('}');
            else
                s.push(']');
           }

        // Si se cierra una expresi�n, tengo que chequear que coincida con la ultima que fue abierta
        if( c == ')' || c == ']' || c == '}' ){

            // Si el stack esta vacio, significa que no hay nada abierto.
            // Por lo tanto, no se puede cerrar nada y la cadena esta desbalanceada
            if( s.empty() == true)
                return false;

            // En caso contrario, debe chequear que se cierre lo ultimo que se abrio
            if( c != s.top())
                return false;
            else
                s.pop();
        }
    } //for

    // Si algo quedo "abierto", la cadena no esta balanceada
    if(s.empty() != true)
        return false;

    // Si logre iterar sobre todo el string y el stack quedo vacio, entonces la cadena esta balanceada.

    return true;
}

string pop_extension(const string& s) {
    string ss = s;
    ss.pop_back(); ss.pop_back(); ss.pop_back(); ss.pop_back();
    return ss;
}

string pop_n(string ss,int n)
{
    while(n--){ ss.pop_back();}
    return ss;
}

int remplace_1(string& my_str, const string& sub_str, const char c) {
    int index;

    index = my_str.find(c);
    my_str.replace(index, 1, sub_str, 0, sub_str.length());

    return index;
}

string remove_spaces(string s, string cads) {
    for (size_t i = 0; i < cads.size(); i++) {
        s.erase(remove(s.begin(), s.end(), cads.at(i)), s.end());
    }
    return s;
}

string get_files_name(const string& name, const char extract, const string& extension) {
    string directory = name;
    directory = pop_n(directory, 4);

    remplace_1(directory, extension, extract); //reemplazo
    return directory;
}

bool is_scientific_notation(char c, char last_c) {
    return (tolower(last_c) == MATH_SIMBOLS[SIMB_e] ) && (c == MATH_SEPARATORS[OP_SUMA] || c == MATH_SEPARATORS[OP_RESTA]);
}

void push_neg_math(Stack<string> *stk) {

    stk->push(MATH_NEG);

    const string product(1, MATH_SEPARATORS[OP_MULT]);// '*'
    stk->push(product);
}

void do_math(Stack<string> *stk, string &acumulador, char c, char last_last_c) {
    //Si es un numero:

    if (c < 57 && c > 48) { //Si es un numero
        if (last_last_c < 57 && last_last_c > 48) {
            return;
        }
        string top = stk->top();
        stk->pop();
        acumulador.append(top);
    }
    else {
        stk->pop();
        if (last_last_c != 0 && containChar(MATH_SEPARATORS, last_last_c)==false) {
            stk->push(string(1,'+'));
        }
        push_neg_math(stk);
    }
}

//
bool validate_double_operator(const char &c, const char &lc, const string &operators) {
    return containChar(operators, c) && containChar(operators, lc);
}

/*La siguiente funcion parte un string en separadores, pero manteniendolos
en donde se encuentren.
Luego los almacena en un stack donde el tope es el primer substring obtenida
y asi siguiendo.
Precondiciones: input y separators deben estar correctamente inicializados
POSTCONDICIONES: Se crea un stack con todas las substrings obtenidas de
input. El stack USA MEMORIA DINAMICA con lo cual debe eliminarse despues de
su uso.
*/

//O(n) Temporal y O(n) Espacial si se cuenta el string de input como O(1) ya que es un sólo string.
Stack<string> * read_math_expression(const string &input)
{
    Stack<string> * stk = new Stack<string>;
    string acumulador; // string donde se iran guardando las substrings de input

    if((containChar(MATH_OPT,input[0])&& input[0] != MATH_SEPARATORS[OP_RESTA] && isdigit(input[1]))
        || containChar(MATH_OPT, input[input.size() - 1])){
        cerr << "ERROR: Math Error." << endl;
        exit(1);
    }

    if(input.empty() == true)// Si el input está vacío
    {
        stk->push(input);
        return stk;
    }

    size_t i = 0;
    char c, last_c, last_last_c;

    //Temporal O(n)
    for (i = 0, last_c=0, last_last_c=0; (c = input[i]) != '\0'; i++, last_last_c = last_c, last_c = c) {

        if (validate_double_operator(c, last_c, MATH_OPT)) {
            cout << "ERROR: Entrada invalida [doble operador]" << endl;
            exit(1);
        }

        if (containChar(MATH_SEPARATORS, c) != true) // Si no es un separador
        {
            if (last_c == MATH_SEPARATORS[OP_RESTA] && tolower(last_last_c) != MATH_SIMBOLS[SIMB_e] && last_last_c != MATH_SEPARATORS[OP_RP] && last_last_c == MATH_SEPARATORS[OP_LP]) { //Si es un numero, lo tomo como negativo, si es una funcion o una variable, la multiplico por (-1)
                do_math(stk, acumulador, c, last_last_c);
            }
            acumulador.push_back(c);
            continue;
        }

        if (acumulador.empty() != true) // si el acumulador no está vacío
        {
            if (is_scientific_notation(c, last_c) == true) { //es una notacion cientifica?
                acumulador.push_back(c);//Pongo el e
                continue;
            }
            acumulador = fullTrim(acumulador);
            stk->push(acumulador); // Push al stack del acumulador con contenido
            acumulador.clear(); //vacio
        }

        //Caso en donde el acumulador esta vacio
        acumulador.push_back(c); // Agrega el separador al string
        stk->push(acumulador); // Agrega el separador al stack
        acumulador.clear(); // Borra el stack
    }

    if(acumulador.empty()!= true)
    {
        acumulador = fullTrim(acumulador);
        stk->push(acumulador); // se almacena el ultimo string
    }

    //Temporal O(n) - Espacial O(1)
    stk->invert(); // se invierte el stack
    return stk;
}

//Complejidad Temporal O(k) - Complejidad Espacial O(1). Donde k es un número constante del diccionario -> O(1)
bool containChar(const string & input, char c)
{
	return (input.find(c) != input.npos)? true: false;
}

string fullTrim(string str){
	char c;
	size_t i =0;

	while(((c = str[i]) == ' ' || c == '\t' || c == '\n') && c != '\0')
		i++;

	str.erase(0,i);// Borra los caracteres blancos desde cero hasta i

	if(c != '\0')
	{
		while( (c = str.back() ==' ' || c == '\t' || c == '\n' ) )// si encontras un caracter blanco, borralo
		{
			str.pop_back();
		}

	}

	return str;
}
