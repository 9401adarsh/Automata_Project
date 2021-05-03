#include<stdio.h>
#include<iostream>
#include<string.h>
#include<stdio.h>
#include<malloc.h>
#include<vector>

using namespace std;

#define nFile 30 //Max 30 characters for file name
#define nLine 400 //Max 400 characters for a line in input file
#define nDigits 2 //Max number of decimal digits
#define bDigits 7 //Max number of binary digits to store all integer states
#define nStates 100 //Max number of states for the Turning Machine Simulator
#define nAlphabets 20 //Max number of alphabets that a Turing Machine Tape can read and write, including Blank

int atoz_map[26];

struct TM_transition_rule
{
    int q; //State Number
    char w_s; //Symbol to write on Tape
    char move; //Move left or right
};

TM_transition_rule* TM_matrix_ptr[nStates][nAlphabets];

char equivalent_character(char c)
{
    //Function to encode ' ' as 'b'
    return (c == ' ')?'b':c;
}

void print_TM_string(TM_transition_rule *rule)
{
    if(!rule) //when no rule exists
    {
        cout<<"     /     ";
    }
    else
    {
        cout<<" q"<<rule->q<<", "<<equivalent_character(rule->w_s)<<","<<rule->move<<"   ";
    }
}

char* FileName = new char[nFile];
char* line = new char[nLine];
char* Alphabet = new char[nAlphabets];
char special_char;

enum SymbolType{ number, lparen, rparen, comma, dash, _q, _b, _l, _r, _char, eol };
FILE *file;
SymbolType symbol;

int pos, //Position of Marker on TM Tape;
    len, //Length of Input File Line
    numVal,  // numeric value of symbolic number
    mStates,  // number of TM states
    mSymbols,  // number of TM symbols (including ' ')
    state,  // global index into TMrule[ state ][ ... ]
    nSimSteps;  // number of simulation steps


char* ErrorCodes[] = { "'q'", "number", "'b' or non-special character","'('", "'l', 'r' or '-'", "')'" };

void SkipBlanks()
{
   while (pos < len && line[ pos ] == ' ')
   {
      ++pos;
   }
}// SkipBlanks

void Error(int i)
{
   for(int j = 0; j < pos-1; j++)
    	cout<<" ";
   cout<<"^\nError "<<i<<": "<<ErrorCodes[i]<<"expected\n";
}//ErrorMsg, when wrong syntax is parsed

bool BlankLine(string line, int len)
{
   bool blank = true;
   for (int i = 0; i < len; i++)
   {
      if (line[i] != ' ')
      {
         blank = false;
         break;
      }
   }
   return blank;
}// BlankLine

int GetLine(char* line)
{
   int commentLine = 1;
   while(commentLine && (fgets((line), nLine, file) != NULL) ) 
   {
      len = strlen( line ) - 1;
      line[ len ] = '\0';
      if (BlankLine(line,len))
      {
         cout<<"";
         continue;
      }
      if (line[0] != ';' )
      {
         commentLine = 0;
      }
      else 
        cout<<line<<endl;
   }
   return len;
}// getLine - prints all Comment Lines, and other descriptive parameters

void nextSymbol(int ScanNumber = 1)
{
    char ch;
    int i;

    SkipBlanks();

    if(pos < len)
    {
        ch = line[pos++];
        if(ScanNumber && isdigit(int(ch)))
        {
            numVal = 0;
            i = 1;
            do
            {
                if(i < nDigits)
                {
                    numVal = 10*numVal + ch - '0';
                }    
                ch = line[pos++];
            } while (isdigit(int(ch)) && pos<len);
            symbol = number;
        }
        else
        {
            switch(tolower(ch))
            {
                case '(': symbol = lparen;  break;
                case ')': symbol = rparen;  break;
                case ',': symbol = comma;   break;
                case '-': symbol = dash;    break;
                case 'q': symbol = _q;      break;
                case 'b': symbol = _b;      ch = ' '; break;
                case 'l': symbol = _l;      break;
                case 'r': symbol = _r;      break;
                default: symbol = _char;   break;
            }
            special_char = ch;
        }
    }
    else
    {
        symbol = eol;
    }
}//ReadCurrentSymbolandAssertType, accordingly parse into transition matrix

int CharIndex(char a)
{
   int j;

   for (j = 0; j < mSymbols; j++)
   {
      if (Alphabet[j] == a)
      {
        break;
      }
   }
   return j;
}// CharIndex - returns column index for character in transition matrix

void TM_S( int *var )
{
   if(symbol == _q) 
   {
      nextSymbol();
      if(symbol == number) 
      {
        *var = numVal;
        nextSymbol(0);
      }
      else Error(1);
   }
   else Error(0);
}// TM_S - Function that initialises the state for the transition function


void TM_T()
{
    char scan_character, rw_character, direction;
    int nextState;
    TM_transition_rule *rule_ptr;

    if(symbol != eol)
    {
        if(symbol == _b || symbol == _char)
        {
            scan_character = special_char;
            nextSymbol();
            if(symbol == lparen)
            {
                nextSymbol();
                TM_S(&nextState);
                if(symbol == _char || symbol == _b)
                {
                    rw_character = special_char;
                    nextSymbol();
                    if(symbol == _l || symbol == _r || symbol == dash)
                    {
                        direction = special_char;
                        rule_ptr = new TM_transition_rule();
                        rule_ptr->q = nextState;
                        rule_ptr->w_s = rw_character;
                        rule_ptr->move = direction;
                        TM_matrix_ptr[state][CharIndex(scan_character)] = rule_ptr;
                        nextSymbol();
                        if(symbol == rparen)
                        {
                            nextSymbol(0);
                            TM_T();
                        }
                        else Error(5);
                    }
                    else Error(4);
                }
                else Error(2);
            }
            else Error(3);
        }
        else Error(2);
    }
}

void TM_P()
{
    TM_S(&state);
    TM_T();
}

void InputTM()
{
    GetLine(line);
    mStates = atoi(line);
    mSymbols = GetLine(Alphabet);
    for(int i = 0; i < nStates; i++)
    {
        for(int j = 0; j < nAlphabets; j++)
            TM_matrix_ptr[i][j] = nullptr;
    }

    cout<<endl;
    while( (len = GetLine(line)) != 0)
    {
        cout<<line<<endl;
        if(line[0] == '*')
            break;
        
        pos = 0;
        if(pos < len)
        {
            nextSymbol();
            TM_P();
        }
    }
}

void ReportTM()
{
   cout<<"Turing Machine for :- "<<FileName<<endl;
   cout<<"States: "<<mStates<<" Alphabet: "<<Alphabet<<"  ,"<<mSymbols<<" Symbols"<<endl;

   cout<<"Transition Matrix for the TM"<<endl;

   for(int j = 0; j < mSymbols; j++)
   {
       cout<<"      "<<equivalent_character(Alphabet[j])<<"    ";
   }

   cout<<endl;
   for(int i = 0; i < mStates; i++)
   {
       cout<<"q"<<i;
       for(int j = 0; j < mSymbols; j++)
       {
           print_TM_string(TM_matrix_ptr[i][j]);
       }
       cout<<endl;
   }

}// ReportTM

void PrintConfiguration(int q, string t, int h)
{
   char c;
   int i;

   for(i = 0; i < h; i++)
      cout<<t[i];
   
   cout<<"|q"<<q<<">";
   i = h;

   while(i<len)
   {
      cout<<t[i++];
   }
   while((c = t[i++])!= ' ')
   {
      cout<<c;
   }
   cout<<endl;
}// PrintConfiguration

int SimulateTM(string ip_string)
{
    if(ip_string[0] == '!')
        return 0;
    
    string tape = ip_string;
    int head, q, n, t;
    TM_transition_rule* rule_ptr;
    long long unsigned int steps;
    for(int i = tape.length(); i < nLine-1; i++)
        tape.push_back(' ');
    tape.push_back('\0');
    cout<<endl;
    head = 0;
    q = 0;
    t = 0;
    steps = 0L;
    cout<<"The Tape Before Simulation"<<endl;
	PrintConfiguration(q, tape, head);
	while(q >= 0)
    {
        //PrintConfiguration(q, tape, head);
        rule_ptr = TM_matrix_ptr[q][CharIndex(tape[head])];
        if(rule_ptr != nullptr)
        {
            steps++;
            q = rule_ptr->q;
            tape[head] = rule_ptr->w_s;
            if(rule_ptr->move == '-')
                break;
            head += (tolower(rule_ptr->move) == 'r')?1:-1;
            if(head < 0 || head >= nLine)
            {
                cout<<"Illegal reference made by the head for position "<<head<<endl;
                q = -1;
            }
            if(nSimSteps > 0)
            {
                t++;
                if(t > nSimSteps)
                    break;
            }
        }
        else
            break;

    }
    cout<<"The Tape after simulation comes to a stop"<<endl;
    PrintConfiguration(q, tape, head);
    if(q == mStates -1)
        cout<<"Input accepted by TM"<<endl;
    else   
        cout<<"Input not accepted by TM"<<endl;
    
    cout<<"--------------------------------------"<<endl;

    string final_result(tape);
    int result = 0;
    for(auto x : final_result) 
    {
        if(x == '0')
            result++;
    }
    
    return result;
}

//Main function to encrypt with updated hash_map
void encrypt_string(string s)
{
    string encrypted_string;
    for(auto x : s)
    {
        if(islower(x))
        {
            int shift = atoz_map[x-'a'];
            encrypted_string.push_back(char('a' + shift));
        }
        else if(isupper(x))
        {
            int shift = atoz_map[x-'A'];
            encrypted_string.push_back(char('A' + shift));
        }
        else
            encrypted_string.push_back(x);
    }   
    cout<<"The encrypted string for "<<s<<" is:-"<<endl;
    cout<<encrypted_string<<endl;

    return;
}

int main()
{
    for(int i = 0; i < 26; i++)
        atoz_map[i] = i;
    
    int shift;
    cout<<"Enter Required Shift"<<endl;
    cin>>shift;

    string shift_delimiter_for_addition;
    shift_delimiter_for_addition.push_back('1');
    for(int i = 1; i <= shift; i++)
        shift_delimiter_for_addition.push_back('0');
    
    vector<string> TM_addition_inputs;
    for(int i = 0; i < 26; i++)
    {
        string s = "";
        for(int j = 0; j < i; j ++)
            s.push_back('0');
        s += shift_delimiter_for_addition;
        TM_addition_inputs.push_back(s);
    }
    
   	FileName = "AdditionTM_in.txt";
    if ( ( file = fopen( FileName, "r" ) ) == NULL )
    {
      printf( "\nunable to open file '%s'\n", FileName );
    }
    else 
    {
      printf( "%s\n", FileName );
      printf( "number of simulation steps [0 = indefinite]? " );
      scanf( "%d", &nSimSteps );
      printf( "%d\n", nSimSteps );
	  cout<<"Establishing TM"<<endl;
      InputTM();
      ReportTM();
      cout<<"TM Reported"<<endl;

      for(int i = 0; i < 26; i++)
            atoz_map[i] = SimulateTM(TM_addition_inputs[i]);
   }

    string delimiter_for_mod;
    delimiter_for_mod.push_back('1');
    for(int i = 0; i < 26; i++)
        delimiter_for_mod.push_back('0');
    
    vector<string> TM_mod_inputs;
    for(int i = 0; i < 26; i++)
    {
        string s = " ";
        for(int j = 0; j < atoz_map[i]; j ++)
            s.push_back('0');
        s += delimiter_for_mod;
        TM_mod_inputs.push_back(s);
    }
    
    FileName = "AmodBTM_in.txt";
    if ( ( file = fopen( FileName, "r" ) ) == NULL )
    {
      printf( "\nunable to open file '%s'\n", FileName );
    }
    else 
    {
      printf( "%s\n", FileName );
      printf( "number of simulation steps [0 = indefinite]? " );
      scanf( "%d", &nSimSteps );
      printf( "%d\n", nSimSteps );
	  cout<<"Establishing TM"<<endl;
      InputTM();
      ReportTM();
      cout<<"TM Reported"<<endl;

      for(int i = 0; i < 26; i++)
            atoz_map[i] = SimulateTM(TM_mod_inputs[i]);
   }

    cout<<"Processing has been done"<<endl<<"-------------------------------"<<endl;
	cout<<"The given shift was "<<shift<<endl;
    cout<<shift<<endl;
	
	cout<<"Encrypted Hash Map"<<endl;
    for(int i = 0; i < 26; i++)
        cout<<atoz_map[i]<<endl;
   
    string ip_string;
    cout<<"Enter String to Encrypt"<<endl;
    cin>>ip_string;
    encrypt_string(ip_string);
    return 0;
}// _tmain
