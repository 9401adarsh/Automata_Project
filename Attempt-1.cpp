#include<bits/stdc++.h>
#include<vector>

using namespace std;

#define nFile 30 //Max 30 characters for file name
#define nLine 400 //Max 400 characters for a line in input file
#define nDigits 2 //Max number of decimal digits
#define bDigits 7 //Max number of binary digits to store all integer states
#define nStates 100 //Max number of states for the Turning Machine Simulator
#define nAlphabets 20 //Max number of alphabets that a Turing Machine Tape can read and write, including Blank

int atoz_map[26];  // Hash map to store final cipher mapping

char* FileName = new char[nFile];
char* line = new char[nLine];
char* Alphabet = new char[nAlphabets];
char special_char;

enum SymbolType{ number, lparen, rparen, _q, _b, _l, _r, _char, eol };
FILE *file;
SymbolType symbol;

int pos, //Position of Marker on TM Tape;
    len, //Length of Input File Line
    numVal,  // numeric value of symbolic number
    mStates,  // number of TM states
    mSymbols,  // number of TM symbols (including ' ')
    state,  // global index into TMrule[ state ][ ... ]
    nSimSteps;  // number of simulation steps


char* ErrorCodes[] = { "'q'", "number", "'b' or non-special character","'('", "'l' or 'r'", "')'" };


struct TM_transition_rule
{
    int q; //State Number-REFERRRING TO THE STATE IN O/P OF TRANSITION FUNCTION
    char w_s; //Symbol to write on Tape
    char move; //Move left or right
};

TM_transition_rule* TM_matrix_ptr[nStates][nAlphabets];


// UTILITY FUNCTIONS FOR READING TM DESCRIPTION FILE -------------------------------------------------------------------------

char read_blank(char c)
{
    //Function to encode ' ' as 'b'
    return (c == ' ')?'b':c;
}

void print_TM_String(TM_transition_rule *rule)
{
    if(!rule) //when no rule exists
    {
        cout<<"     /     ";
    }
    else
    {
        cout<<" q"<<rule->q<<", "<<read_blank(rule->w_s)<<","<<rule->move<<"   ";
    }
}


void SkipBlanks()  // Skip blanks while reading transition function file
{
   while (pos < len && line[ pos ] == ' ')
   {
      ++pos;
   }
}

void Error(int i)  // To print syntax error while reading file 
{
   for(int j = 0; j < pos-1; j++)
    	cout<<" ";
   cout<<"^\nError "<<i<<": "<<ErrorCodes[i]<<"expected\n";
}

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
}

int GetLine(char* line)         // getLine - prints all Comment Lines, and other descriptive parameters
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
}

void nextSymbol(int ScanNumber = 1)     //ReadCurrentSymbolandAssertType, accordingly parse into transition matrix
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
}

int CharIndex(char a)       // CharIndex - returns column index for character in transition matrix
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
}


// FUNCTIONS TO INITIALIZE AND SIMULATE TURING MACHINE -------------------------------------------------------------------

void TM_State( int *var )       // Utility function to initialise state for transition function
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
}


void TM_Transition()   // Utility fn to intialize transitions for a given state
{
    char character_read, rw_character, direction;
    int nextState;
    TM_transition_rule *rule_ptr;

    if(symbol != eol)
    {
        if(symbol == _b || symbol == _char)
        {
            character_read = special_char;
            nextSymbol();
            if(symbol == lparen)
            {
                nextSymbol();
                TM_State(&nextState);
                if(symbol == _char || symbol == _b)
                {
                    rw_character = special_char;
                    nextSymbol();
                    if(symbol == _l || symbol == _r)
                    {
                        direction = special_char;
                        rule_ptr = new TM_transition_rule();
                        rule_ptr->q = nextState;
                        rule_ptr->w_s = rw_character;
                        rule_ptr->move = direction;
                        TM_matrix_ptr[state][CharIndex(character_read)] = rule_ptr;
                        nextSymbol();
                        if(symbol == rparen)
                        {
                            nextSymbol(0);
                            TM_Transition();
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

void TM_Init()
{
    TM_State(&state);
    TM_Transition();
}

void InputTM()  // To read description of TM from text file
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
            TM_Init();
        }
    }
}

void ReportTM()     // To display description of TM
{
   cout<<"Turing Machine for :- "<<FileName<<endl;
   cout<<"States: "<<mStates<<" Alphabet: "<<Alphabet<<"  ,"<<mSymbols<<" Symbols"<<endl;

   cout<<"Transition Matrix for the TM"<<endl;

   for(int j = 0; j < mSymbols; j++)
   {
       cout<<"      "<<read_blank(Alphabet[j])<<"    ";
   }

   cout<<endl;
   for(int i = 0; i < mStates; i++)
   {
       cout<<"q"<<i;
       for(int j = 0; j < mSymbols; j++)
       {
           print_TM_String(TM_matrix_ptr[i][j]);
       }
       cout<<endl;
   }

}

void PrintConfiguration(int q, string t, int h)     //Utility function to print configuration of TM after each step
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
}

int SimulateTM(string ip_string)
{   
    string tape = ip_string;
    int head, q, n, t;
    TM_transition_rule* rule_ptr;
    long long unsigned int steps;

    for(int i = tape.length(); i < nLine-1; i++)  // initlaizing tape with blanks
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
        rule_ptr = TM_matrix_ptr[q][CharIndex(tape[head])];
        if(rule_ptr != nullptr)
        {
            steps++;
            q = rule_ptr->q;
            tape[head] = rule_ptr->w_s;                     // storing the current step

            head += (tolower(rule_ptr->move) == 'r')?1:-1;  // moving left or right depending on transition rule

            if(head < 0 || head >= nLine)                  // tape overflow
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
        else if (x=='1'){
            result=0;
        }
        
    }
    
    return result;
}

// FUNCTION TO ENCRYPT INPUT STRING WITH GENERATED HASH MAP ----------------------------------------------------------------------------------------

void encrypt_string(string s)
{
    string encrypted;
    for(auto x : s)
    {
        if(islower(x))
        {
            int shift = atoz_map[x-'a'];
            encrypted.push_back(char('a' + shift));
        }
        else if(isupper(x))
        {
            int shift = atoz_map[x-'A'];
            encrypted.push_back(char('A' + shift));
        }
        else
            encrypted.push_back(x);
    }   
    cout<<"The encrypted string for "<<s<<" is:-"<<endl;
    cout<<encrypted<<endl;

    return;
}


//main function
int main()
{
    for(int i = 0; i < 26; i++)
        atoz_map[i] = i;
    
    int shift;
    cout<<"Enter Required Shift"<<endl;
    cin>>shift;

    // Addition TM, to shift character places for cipher-----------------------------------------------------------------

    //Formatting input string like 000100 where the 000 stands for c and 00 stands for 2 places of shift

    string addition_delim;
    addition_delim.push_back('1');
    for(int i = 1; i <= shift; i++)
        addition_delim.push_back('0');
    
    vector<string> TM_addition_inputs;
    for(int i = 0; i < 26; i++)
    {
        string s = "";
        for(int j = 0; j < i; j ++)
            s.push_back('0');
        s += addition_delim;
        TM_addition_inputs.push_back(s);
    }
    
    //opening input file that contains the TM transition table for execution
   	FileName = "AdditionTM_in.txt";

    if ( ( file = fopen( FileName, "r" ) ) == NULL )
    {
      cout<<"\nunable to open file"<<FileName<<"\n";
    }
    else 
    {
      cout<<FileName<<endl;
      cout<<"number of simulation steps [0 = indefinite]? ";
      cin>>nSimSteps;
      cout<<nSimSteps<<"\n";
	  cout<<"Establishing TM"<<endl;

      InputTM();
      ReportTM();
      cout<<"TM Reported"<<endl;

      for(int i = 0; i < 26; i++)
            atoz_map[i] = SimulateTM(TM_addition_inputs[i]);
   }

   // Modulus TM, for calculating mod in case of overflow ----------------------------------------------------------------------

    string mod_delim;
    mod_delim.push_back('1');
    for(int i = 0; i < 26; i++)
        mod_delim.push_back('0');
    
    vector<string> TM_mod_inputs;
    for(int i = 0; i < 26; i++)
    {
        string s = " ";
        for(int j = 0; j < atoz_map[i]; j ++)
            s.push_back('0');
        s += mod_delim;
        TM_mod_inputs.push_back(s);
    }
    
    FileName = "AmodBTM_in.txt";

    if ( ( file = fopen( FileName, "r" ) ) == NULL )
    {
      cout<<"\nunable to open file "<<FileName<<"\n";
    }
    else 
    {
      cout<<FileName<<endl;
      cout<<"number of simulation steps [0 = indefinite]? ";
      cin>>nSimSteps;
      cout<<nSimSteps<<"\n";
	  cout<<"Establishing TM"<<endl;

      InputTM();
      ReportTM();
      cout<<"TM Reported"<<endl;

      for(int i = 0; i < 26; i++)
            atoz_map[i] = SimulateTM(TM_mod_inputs[i]);
   }

    cout<<"Processing has been done"<<endl<<"-------------------------------"<<endl;
    cout<<"The given shift was "<<shift<<endl;
    // Printing encrypted string ---------------------------------------------------------------------------------------------
	
	cout<<"Encrypted Hash Map"<<endl;
    for(int i = 0; i < 26; i++)
        cout<<atoz_map[i]<<endl;
   
    cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n'); // flushing
    string ip_string;
    cout<<"Enter String to Encrypt: ";
    getline(cin,ip_string);

    encrypt_string(string(ip_string));      // Display final string

    return 0;
}// _tmain
