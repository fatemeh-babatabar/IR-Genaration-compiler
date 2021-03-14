#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <string>
#include <cctype>
#include <cstring>
#include <stdio.h>

using namespace std;

///file
ifstream myfile;
ofstream outfile;
bool endfile=false;
int scope=0;
/////
int line=1;
int label =1;
int reg=0;
//count of error
int count_error=0;

//////////
struct table {
    string var ;
    int type;
    bool isFunc=false;
    int numOfParams;
    int reg=-1;
    vector<table> list ;
    bool R_Reg=false;
    int scop=0;
    ///e
    vector<double> value;
    ///e
    int len=value.size();
};

////
struct exprVal {
    string tok ;
    int type;
    int reg=-1;
    //e
    vector<double> val;
    //e
    int len = val.size();
    //e
    int lenReg;
    //e
    int pos=0;
    bool check_for_second_time=false;
    bool check_for_second_time_and=false;
    bool check_for_second_time_or=false;
    int lab;
};

// token
bool drop=true;
string token="";
string T;

//count main
int count_main=0;

//count main
int count;

//int numofparams for each function
int nparams=0;

// index indentifier
int index_iden;

//type
int type_num=1;
int type_list=2;
int type_nil=3;

///
bool return_func=false;

/// functions
void MFile();
bool is_number(string s);
bool is_word(string w);
bool is_signes(string s);
bool isKey(string word);
void ungetcharr();
char getcharr();
string NextToken();
bool is_identifier_char(char ch);
void droptoken();
string gettoken();
void prog();
void func();
void body(int type , int scopp);
void stmt(int type , int scoppp);
void defvar(int scop3);
exprVal expr();
exprVal or_expr();
exprVal and_expr();
exprVal equality_expr();
exprVal relation_expr();
exprVal add_expr();
exprVal mul_expr();
exprVal unary_expr();
exprVal postfix_expr();
exprVal primary_expr();
int clist(int indexFunc , int countp , int &lReg , bool &first_p , int &N ,string func_name , int &last_param );
int flist(vector<table> &LISTT);
bool searchExistIden(string T);
int searchIndexIden(string T);
int Type(string c);
int Type_function(string c);
bool searchIsFunc(string T );
int searchIsFunc_index(string T);
int search(string y);

/////
bool Main(string word);
bool IF(string word);
bool While(string word);
bool For(string word);
bool Else(string word);
bool In(string word);
bool Num(string word);
bool Return(string word);
bool numread(string word);
bool numprint(string word);
bool MakeList(string word);
bool Listlen(string word);
bool Exit(string word);

//generate
void generateCodeForAddorSub(exprVal &tok1 , exprVal &tok2 , string s);
void generateCodeForMul_Div_Mod(exprVal &tok1 , exprVal &tok2 , string s);
void generateCodeFor_cmp_equle(exprVal &tok1 , exprVal &tok2 , string s);
void generateCodeFor_cmp_smaller_or_greater(exprVal &tok1 , exprVal &tok2 , string s);
// symbolTable
vector<table> symbolTable;
/// main
int main(){
    
    MFile();
    
    return 0;
}

/// gettoken;
string gettoken(){
      string o;
      if(myfile.good() && drop==true && !endfile){
        o=NextToken() ;
        while((o=="" || o==" " || o=="\n" || o=="\r") && myfile.good() && !myfile.eof()){
            o=gettoken();
        }
        drop=false;
        token=o;
        return o;
      }else if(!myfile.good()){
          drop=false;
          endfile=true;
      }
      return token;
}

///droptoken
void droptoken(){
    drop=true;
}

///file
void MFile(){
    outfile.open("irCode.txt" , ios::out);
    if(!outfile){
        cout<<"error in opening file"<<endl;
    }
    myfile.open("file.txt")   ;
    if(myfile.good() && outfile){
        
        //
        table numRead;
        numRead.var="numread";
        numRead.type=type_num;
        numRead.isFunc=true;
        numRead.numOfParams=0;
        //
        table numPrint;
        numPrint.var="numprint";
        numPrint.type=0;
        numPrint.isFunc=true;
        numPrint.numOfParams=1;
        //
        table input;
        input.var="input";
        input.type=type_num;
        input.isFunc=false;
        input.numOfParams=0;

        numPrint.list.push_back(input);
        //
        table makelist;
        makelist.var="makelist";
        makelist.type=type_list;
        makelist.isFunc=true;
        makelist.numOfParams=1;
        //
        table number;
        number.var="number";
        number.type=type_num;
        number.isFunc=false;
        number.numOfParams=0;

        makelist.list.push_back(number);
        //
        table listlen;
        listlen.var="listlen";
        listlen.type=type_num;
        listlen.isFunc=true;
        listlen.numOfParams=1;
        //
        table listt;
        listt.var="list";
        listt.type=type_list;
        listt.isFunc=false;
        listt.numOfParams=0;

        listlen.list.push_back(listt);
        //
        table Exit;
        listt.var="exit";
        listt.type=0;
        listt.isFunc=true;
        listt.numOfParams=1;

        Exit.list.push_back(input);

        symbolTable.push_back(numRead);
        symbolTable.push_back(numPrint);
        symbolTable.push_back(makelist);
        symbolTable.push_back(listlen);
        symbolTable.push_back(Exit);

        if(myfile.good()){
             T=gettoken();
             if(!myfile.eof()){
                 prog();
             }else{
                 cout<<"file is empty\n";
             }
        }    
        cout<<"count of errors : "<<count_error<<endl;
        myfile.close();
    }else{
        cout<<"file is not open!";
    }
}

//prog
void prog(){
     reg=0;
     func();
     if(myfile.good()){
        droptoken();
        T=gettoken();
        if(is_word(T) || Main(T)){
             droptoken();
             prog();
        }else if(isKey(T)){
             droptoken();
             cout<<"line "<<line <<" :  error\n";
             count_error++;
             ////////////////////////////
             prog();
        }
     }
     
}
//func
void func(){
    reg=0;
    if(is_word(T)){
         int T_type=Type_function(T);
         if(T_type!=type_num && T_type!=type_list && T_type!=type_nil){
            cout<<"line "<<line <<" : type  "<<T<<" not exist \n";
            count_error++;
         }
         if(myfile.good()){
            droptoken();
            T=gettoken();
            if(is_word(T) || Main(T)){
                string funcName=T;  
                
                bool exist = searchIsFunc(T );
                int index = searchIsFunc_index(T);
                if(Main(T) && count_main==0){
                    count_main++;
                    exist=false;
                }
                int symbolSize;
                if(exist){
                    symbolSize= symbolTable.size();
                    cout<<"line "<<line <<" : "<<T<<"  function declared before or function name same as if , while , for .... \n";
                    count_error++;
                }   
                outfile<<"proc "<< T<<endl;
                table s;
                s.var=T;
                s.isFunc=true;
                s.type=T_type;
                if(Main(T) && T_type!=type_num){
                    cout<<"line "<<line <<" : "<<"type of main function should be num \n";
                    count_error++;
                }
                if(myfile.good()){
                    droptoken();
                    T=gettoken();
                    if(T=="("){
                        if(myfile.good()){
                            droptoken();
                            T=gettoken();
                            nparams=0;
                            s.numOfParams=flist(s.list);  
                            if(Main(funcName) && nparams>0){
                                cout<<"line "<<line<<" : main function should'nt have input parametrs\n";
                                count_error++;
                            }
                         
                            if(T==")"){
                                symbolTable.push_back(s);
                                if(!exist){
                                    symbolSize= symbolTable.size();
                                }
                                for(int j=0 ;j<s.list.size(); j++){
                                    symbolTable.push_back(s.list[j]);
                                }

                                if(myfile.good()){
                                    droptoken();
                                    T=gettoken();
                                    if(T=="{"){
                                        if(myfile.good()){
                                            droptoken();
                                            T=gettoken();
                                            droptoken();
                                            return_func==false;
                                            scope++;
                                            int scope3=scope;
                                            body(T_type , scope3);
                                            if(T!="}"){
                                                cout<<"line "<<line <<"  :} is missing \n";
                                                count_error++;
                                                droptoken();
                                            }

                                            if((T_type==type_list || T_type==type_num) && return_func==false){
                                                cout<< funcName<<" function  should have return value\n";
                                                count_error++;
                                            }
                                            for(int y=0; y<symbolTable.size();y++){
                                                if(symbolTable[y].scop==scope3 && symbolTable[y].type==type_list){
                                                    outfile<<"\tcall rel"<<" , r"<<symbolTable[y].reg<<endl;         
                                                }
                                            }
                                            while(symbolSize != symbolTable.size()){
                                                symbolTable.erase(symbolTable.end());
                                            }
                                        }
                                    }else{
                                        cout<<"line "<<line <<" :  body should be declear in {}\n";
                                        count_error++;
                                    }
                                }
                            }else{
                                cout<<"line "<<line <<" :  not execpt )\n";        
                                count_error++;
                            }
                        }
                    }else{
                        cout<<"line "<<line <<" : not execpt (\n";
                        count_error++;
                    }
                }
            }else if(isKey(T)){
                droptoken();
                cout<<"line "<<line <<" : key word can be redfined \n";
                 count_error++;
            }else{
                cout<<"line "<<line <<" : is ambiguse\n";
                count_error++;
            }     
        }
    }else{
         cout<<"line "<<line <<" :name of type of function  shoulde be word \n";
         count_error++;
    }
}

//body
void body(int type , int scopp){
    if(T!="}" && myfile.good()){
       stmt(type,scopp);
    }
    
    while( myfile.good()  &&  T!="}" ){
        T=gettoken();
        droptoken();
        stmt(type,scopp);
    }
     
}

//stmt
void stmt(int type,int scoppp){
    if(IF(T)){
        if(myfile.good()){
            droptoken();
            T=gettoken();
            if(T=="("){
                if(myfile.good()){
                    droptoken();
                    T=gettoken();
             
                    int t_label=label;
                    exprVal exp=expr();
                    if(exp.type==type_list){
                        cout<<"line "<<line<<" : illegal condation type!";
                        count_error++;
                    }

                    outfile<<"\tjz r"<<exp.reg<<" , lab"<<t_label<<endl;
                  
                    label++;
                    int t_label2=label;
                    if(T==")"){
                        if(myfile.good()){
                            droptoken();
                            T=gettoken();
                            stmt(type , scoppp);
                            if(Else(T)){
                                outfile<<"\tjmp lab"<<t_label2<<endl;
                                outfile<<"lab"<<t_label<<":\n";
                                if(myfile.good()){
                                    droptoken();
                                    T=gettoken();
                                    stmt(type,scoppp);
                                }
                                outfile<<"lab"<<t_label2<<":\n";
                            }else{
                                outfile<<"lab"<<t_label<<":\n";
                            }
                            label++;
                         
                        }
                    }else{
                        cout<<"line "<<line << " : error )\n";    
                        count_error++;
                    }
                }
            }else{
                cout<<"line "<<line << " : error (\n";
                count_error++;
            }
        }
    }else if(While(T)){
        if(myfile.good()){
            droptoken();
            T=gettoken();
            if(T=="("){
                if(myfile.good()){
                    droptoken();
                    T=gettoken();
                    outfile<<"lab"<<label<<":\n";
                    int t_label=label;
                    exprVal exp=expr();

                    if(exp.type==type_list){
                        cout<<"line "<<line<<" : illegal condation type!";
                        count_error++;
                    }
                
                    
                    label++;
                    outfile<<"\tjz r"<<exp.reg<<" , lab"<<label<<"\n";
                    
                    
                    ////////////////////////////////////////
                    if(T==")"){
                        if(myfile.good()){
                            droptoken();
                            T=gettoken();
                            stmt(type,scoppp);
                            outfile<<"\tjmp lab"<<t_label<<endl;
                            outfile<<"lab"<<label<<":"<<endl;
                        }

                    }else{
                        cout<<"line "<<line << " : error )\n";    
                        count_error++;
                    }
                }
            }else{
                cout<<"line "<<line << " : error (\n";
                count_error++;
            }
        }
    }else if(For(T)){
        if(myfile.good()){
            droptoken();
            T=gettoken();
        //    cout<<T<<"  oo\n";
            if(T=="("){
                if(myfile.good()){
                    droptoken();
                    T=gettoken();

                    if(is_word(T)){
                        bool isIden=searchExistIden(T);
                        int identifier=index_iden;
                        int first_reg=-1;
                        if(isIden && symbolTable[identifier].type==type_num){
                            if(!symbolTable[identifier].R_Reg){
                                symbolTable[index_iden].R_Reg=true;
                        
                                symbolTable[identifier].value.push_back(0);
                                symbolTable[identifier].reg=reg;
                                first_reg=reg;
                                outfile<<"\tmov r"<<reg<<" , 0"<<"\n";
                                reg++;
                                symbolTable[identifier].reg=reg;
                                reg++;
                            }else{
                                first_reg=symbolTable[identifier].reg;
                                outfile<<"\tmov r"<<first_reg<<" , 0"<<"\n";
                                reg++;
                                symbolTable[identifier].reg=reg;
                                
                            }
                            
                        }
                        bool correct=true;
                        int t_label;
                        int t_label_end;
                        int reg2;
                        if(!isIden){
                            cout<<"line "<<line<<" : identifier "<< T<<" is not exist \n";
                            count_error++;
                            correct=false;
                        }else if(symbolTable[index_iden].type!=type_num){
                            cout<<"line "<<line<<" : type of identifier "<< T<<" shoulde be num \n";
                            count_error++;
                            correct=false;
                        }
                        if(myfile.good()){
                            droptoken();
                            T=gettoken();
                            if(In(T)){
                                if(myfile.good()){
                                    droptoken();
                                    T=gettoken();
                                    exprVal exp=expr();
                                    int ident;
                                    if(exp.type!=-1){
                                        ident = searchIndexIden(exp.tok);
                                        if(symbolTable[ident].type!=type_list){
                                            cout<<"line "<<line<<" : type of identifier "<< T<<" shoulde be list \n";
                                            correct=false;
                                        }else{
                                       
                                            reg2=reg;
                                            reg++;
                                          
                                            outfile<<"\tmov r"<<reg<<" , "<<symbolTable[ident].len<<endl;
                                     
                                            outfile<<"lab"<<label<<":\n";
                                            t_label=label;
                                            reg++;
                                            outfile<<"\tcmp< r"<<reg<<" , r"<<first_reg<<" , r"<<reg-1<<endl;
                                            label++;
                                            t_label_end=label;
                                            outfile<<"\tjz r"<<reg<<" , lab"<<label<<endl;
                                            reg++;
                                            outfile<<"\tmov r"<<reg<<" , 1"<<endl;
                                            outfile<<"\tadd r"<<reg<<" , r"<<reg<<" ,r"<<first_reg<<endl;
                                            reg++;
                                            outfile<<"\tmov r"<<reg<<" , 8"<<endl;
                                            outfile<<"\tmul r"<<reg<<" , r"<<reg-1<<" ,r"<<reg<<endl;
                                            outfile<<"\tadd r"<<symbolTable[ident].reg<<" , r"<<symbolTable[ident].reg<<" ,r"<<reg<<endl;
                                       
                                            outfile<<"\tmov r"<<symbolTable[identifier].reg<<" , r"<<symbolTable[ident].reg<<endl;
                                            outfile<<"\tsub r"<<symbolTable[ident].reg<<" , r"<<symbolTable[ident].reg<<" ,r"<<reg<<endl;
                                       }
                                    }else{
                                        correct=false;
                                    }
                                    if(T==")"){
                                        if(myfile.good()){
                                            droptoken();
                                            T=gettoken();
                                            stmt(type, scoppp);
                                       
                                            reg++;
                                            outfile<<"\tmov r"<<reg<<" , 1"<<endl;
                                            outfile<<"\tadd r"<<first_reg<<" , r"<<first_reg<<" ,r"<<reg<<endl;
                                            outfile<<"\tjmp lab"<<t_label<<endl;
                                            outfile<<"lab"<<t_label_end<<":"<<endl;
                                        }
                                    }else{
                                        cout<<"line "<<line << " : error )\n";
                                        count_error++;
                                    }
                                }
                            }else{
                               cout<<"line "<<line << " : error in for \n";
                               count_error++;
                            }
                        }
                    }else{
                        cout<<"line "<<line << " : rong  iden for iden in for\n";
                        count_error++;
                    }
                }      
            }else{
                cout<<"line "<<line << " : error (\n";
                count_error++;
            }
        }
    }else if(Return(T)){
      if(type==type_nil){
            cout<<"line "<<line << " : this function shouldent have return value \n";                                
            count_error++;
      }  
        return_func=true;
        if(myfile.good()){
            droptoken();
            T=gettoken();

            exprVal type_ret=expr();
            if(type!=type_ret.type && type!=type_nil){
                cout<<"line "<<line << " : the return type isnt same as function type \n";
                count_error++;
            }
            if(T!=";"){
                cout<<"line "<<line << " : error ; \n";
                count_error++;
            }
            if(type_ret.reg!=0){
                outfile<<"\tmov r0 , r"<<type_ret.reg<<endl;
            }
            
            outfile<<"\tret"<<endl;
            if(myfile.good()){
                droptoken();
                T=gettoken();
            }
        }
    }else if(T=="{"){
        scope++;
        int scopp=scope;
        int count_iden_F_I_W=symbolTable.size();
        if(myfile.good()){
            droptoken();
            T=gettoken();
            body(type , scopp);
        }
        if(T!="}"){
            cout<<"line "<<line << " : error } \n";
            count_error++;
        }
        for(int h=0;h<symbolTable.size();h++){
            if(symbolTable[h].scop==scopp && symbolTable[h].type==type_list){
                outfile<<"\tcall rel"<<" , r"<<symbolTable[h].reg<<endl;         
            }
        }
        while(count_iden_F_I_W != symbolTable.size()){
            symbolTable.erase(symbolTable.end());
        }

        if(myfile.good()){
            droptoken();
            T=gettoken();
        }
    }else{
        int y =Type(T); 
        if(y==1 || y==2){
            defvar(scoppp);
            if(myfile.good()){
                T=gettoken();
                if(T!=";"){
                    cout<<"line "<<line << " : error ; \n";
                    count_error++;
                }else{
                    if(myfile.good()){
                        droptoken();
                        T=gettoken();
                    }
                }
            }
        }else{
           exprVal t= expr();
            if(T!=";"){
                cout<<" line "<<line << " : error ; \n";
                count_error++;
            }else{
                if(myfile.good()){
                    droptoken();
                    T=gettoken();
                }
            }    
        } 
    } 
       
}

//defvar
void defvar(int scop3){
    int T_type=Type(T);
    if(T_type!=type_num && T_type!=type_list){
        cout<<"line "<<line <<" : type  "<<T<<" not exist \n";
        count_error++;
    }
    if(myfile.good()){
        droptoken();
        T=gettoken();
        droptoken();

        if(!is_word(T)){
            cout<<"line "<<line<<" : identifire name shoulde be word\n";
            count_error++;
        }
        if(isKey(T)){
            cout<<"line "<<line<<" :  identifire name sholudent be key value\n";
            count_error++;
        }

        if(!searchExistIden(T)){
            table h;
            h.isFunc=false;
            h.var=T;
            h.type=T_type;
            h.numOfParams=0;
            h.scop=scop3;
            symbolTable.push_back(h);
            
        }else{
            cout<<"line "<<line<<" : identifier  "<< T<<"   declared before\n";
            count_error++;
        }
    }
}

//expr
exprVal expr(){
    exprVal t1=or_expr();
    int y=t1.type;
    while(myfile.good()  &&  T=="="){

        int index_i3=search(t1.tok);
        droptoken();
        T=gettoken();
        exprVal t2=or_expr();
        if(t2.type!=y && y!=-1 && t2.type!=-1 ){
         
            cout<<"line "<<line<<" : illegal assignment!  = \n";
            count_error++;
        }else if(index_i3!=-1 && y!=-1  && t2.type!=-1){
            if(symbolTable[index_i3].isFunc){
                cout<<"line "<<line <<" :cant assigne to a function!"<<endl;
            }else{
                
                //////////////////////////////////////////
                symbolTable[index_i3].value=t2.val;
                /////////////////////////////////////////////
                
                if(t1.type==type_num  && symbolTable[index_i3].type==type_list){
             
                    outfile<<"\tst r"<<t2.reg<<" ,r"<<t1.reg<<endl;
                    symbolTable[index_i3].value[t1.pos]=t2.val[t2.pos];
                }else if(t1.type==type_list){
                    ///////////////////////////////////////////////
                    symbolTable[index_i3].len=t1.val.size();
                    //////////////////////////////////////////////
                    
                    if(MakeList(t2.tok)){
                       
                        for(int k=symbolTable[index_i3].value.size();k<=t2.len;k++){
                            symbolTable[index_i3].value.push_back(0);
                        }
                        symbolTable[index_i3].len=t2.len;
            
                        reg++;
                        outfile<<"\tmov r"<<reg<<" , 8"<<endl;
                        outfile<<"\tmul r"<<t2.reg<<" , r"<<t2.reg<<" , r"<<reg<<endl;
                        outfile<<"\tcall mem , r"<<t2.reg<<endl;
                        outfile<<"\tmov r"<<symbolTable[index_i3].reg<<" ,r"<<t2.reg<<endl;
                        
                    }else if(is_word(t2.tok)){
                        outfile<<"\tmov r"<<symbolTable[index_i3].reg<<" ,r"<<t2.reg<<endl;
                    }
                }else if(numread(t2.tok)){
                    outfile<<"\tcall iget , r"<<t1.reg<<endl;
                }else{
                    
                    outfile<<"\tmov r"<<symbolTable[index_i3].reg<<" ,r"<<t2.reg<<endl;
                }
              
                t1.val=t2.val;
            
            }
        }
        y=t2.type;
    }
    return t1;
}

///////////////////////////////////////////////////////////////////////////////////
exprVal or_expr(){
     exprVal t1=and_expr();
     bool check_t1=true;
     bool check_t2=true;
     if(!t1.check_for_second_time_or && T=="||" && t1.type!=-1 && t1.type!=type_list){
                reg++;
                outfile<<"\tmov r"<<reg<<" , 1"<<endl;
                label++;
                t1.lab=label;
                outfile<<"\tjnz r"<<t1.reg<<" , lab"<<t1.lab<<"\n";
                t1.check_for_second_time_or=true;
                t1.check_for_second_time_and=false;
                t1.reg=reg;
    }
    if(t1.type!=-1 && t1.type==type_list && T=="||"){
            cout<<"line : "<<line<<"  illegal assignment!  || \n";
            count_error++;        
    }

     while(myfile.good()  &&  T=="||"){
        droptoken();
        T=gettoken();
        exprVal t2=and_expr();
        if(t2.type!=-1 && t2.type==type_list){
            cout<<"line : "<<line<<"  illegal assignment!  || \n";
            count_error++;
            check_t2=false;
        }
        if(t2.type==-1){
            check_t2=false;
        }

        if(check_t2 && T=="||"){
            outfile<<"\tjnz r"<<t2.reg<<" , lab"<<t1.lab<<"\n";
        }else if(T!="||" && check_t2){
                outfile<<"\tmov r"<<t1.reg<<" , r"<<t2.reg<<endl;
                t1.val.at(t1.pos)=t2.val.at(t2.pos);
                outfile<<"lab"<<t1.lab<<":\n";
        }
     }
     return t1;
}
exprVal and_expr(){
     
     exprVal t1=equality_expr();
     bool check_t1=true;
     bool check_t2=true;
     if(!t1.check_for_second_time_and && T=="&&" && t1.type!=-1 && t1.type!=type_list){
                reg++;
                outfile<<"\tmov r"<<reg<<" , 0"<<endl;
                
                label++;
                t1.lab=label;
            
                outfile<<"\tjz r"<<t1.reg<<" , lab"<<t1.lab<<"\n";

                t1.check_for_second_time_and=true;
                t1.check_for_second_time_or=false;
                t1.reg=reg;
    }
    if(t1.type!=-1 && t1.type==type_list && T=="&&"){
            cout<<"line : "<<line<<"  illegal assignment!  && \n";
            count_error++;        
    }
     while(myfile.good()  &&  T=="&&"){

        
        droptoken();
        T=gettoken();
        exprVal t2=equality_expr();

        if(t2.type!=-1 && t2.type==type_list){
            cout<<"line : "<<line<<"  illegal assignment!  && \n";
            count_error++;
            check_t2=false;
        }
        if(t2.type==-1){
            check_t2=false;
        }

        if(check_t2 && T=="&&"){
            outfile<<"\tjz r"<<t2.reg<<" , lab"<<t1.lab<<"\n";
        }else if(T!="&&" && check_t2){
                outfile<<"\tmov r"<<t1.reg<<" , r"<<t2.reg<<endl;
                t1.val.at(t1.pos)=t2.val.at(t2.pos);
                outfile<<"lab"<<t1.lab<<":\n";
        }
        
     }
     return t1;
}

exprVal equality_expr(){
  
     exprVal t1=relation_expr();
     
     bool check_t1=true;
     bool check_t2=true;

     while(myfile.good()  &&  (T=="=="  ||  T=="!=")){
        string s=T;
        droptoken();
        T=gettoken();

        exprVal t2=relation_expr();
        check_t2=true;
        check_t1=true;

        if(t1.type!=t2.type  &&  t1.type!=-1  && t2.type!=-1){
            t1.type=-1;
            cout<<"line "<<line<<" : illegal assignment!  == or != \n";
            count_error++;
            check_t2=false;
            check_t1=false;
        }

        if(t2.type==-1){
            check_t2=false;
        }

        if(t1.type==-1){
             check_t1=false;
        }
    
        int index_i_t1=search(t1.tok);
        int value=0;
        int k=0;
        if(check_t1 && check_t2){
            ///////////
            if(!t1.check_for_second_time){
                
            }
            reg++;
            if((index_i_t1!=-1 || is_number(t1.tok)) ){
            
                if(is_number(t1.tok)){  
                    value=stoi(t1.tok);
                }else{
                    value=symbolTable[index_i_t1].value.at(t1.pos);
                }

                if(s=="=="){
                    if(value==t2.val.at(t2.pos)){
                        k=1;
                    }else{
                        k=0;
                    }
                }else{
                    if(value!=t2.val.at(t2.pos)){
                        k=1;
                    }else{
                        k=0;
                    }
                }
                generateCodeFor_cmp_equle(t1,t2,s);   
            }    
            ////////////////////////////////////
        }
        bool check=t1.check_for_second_time;
      
        t2.reg=reg;
        if(check){
       
            if(t1.val.at(t1.pos)==k){
                t2.val.at(t2.pos)=k;
            }else{
                t2.val.at(t2.pos)=0;
            }
            t2.check_for_second_time=true;
        }else{
            t2.val.at(t2.pos)=k;
            t2.check_for_second_time=true;
        }
        t1=t2;
        
     }
     return t1;
}
void generateCodeFor_cmp_equle(exprVal &tok1 , exprVal &tok2 , string s){
     if(is_word(tok1.tok) || isKey(tok1.tok)){
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="=="){
                outfile<<"\tcmp==  r"<< reg << ",";
            }else{
                outfile<<"\tcmp!=  r"<< reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
        }else{
            if(s=="=="){
                outfile<<"\tcmp==  r"<< reg << ",";
            }else{
                outfile<<"\tcmp!=  r"<< reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
         
        }   
    }else{
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="=="){
                outfile<<"\tcmp==  r"<< reg << ",";
            }else{
                outfile<<"\tcmp!=  r"<< reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" << tok2.reg <<endl;
        }else{
            if(s=="=="){
                outfile<<"\tcmp==  r"<< reg << ",";
            }else{
                outfile<<"\tcmp!=  r"<< reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
           
        }   
    }
}

exprVal relation_expr(){
     exprVal t1=add_expr();
     bool check_t1=true;
     bool check_t2=true;

     while(myfile.good()  &&  (T=="<"  ||  T==">"  ||  T=="<=" || T==">=")){
        string s=T;
        droptoken();
        T=gettoken(); 

        exprVal t2=add_expr();
        check_t2=true;
        check_t1=true;

        if(t1.type!=t2.type  &&  t1.type!=-1  && t2.type!=-1){
            t1.type=-1;
            cout<<"line "<<line<<" : illegal assignment!  > or >= or < or <= \n";
            count_error++;
            check_t2=false;
            check_t1=false;
        }
         if(t2.type==-1){
            check_t2=false;
        }

        if(t1.type==-1){
             check_t1=false;
        }
    
        int index_i_t1=search(t1.tok);
        int value=0;
        int k=0;
        if(check_t1 && check_t2){
            if(!t1.check_for_second_time){
        
            }
            reg++;
            ///////////
            if((index_i_t1!=-1 || is_number(t1.tok)) ){
                if(is_number(t1.tok)){  
                    value=stoi(t1.tok);
                }else{
                    value=symbolTable[index_i_t1].value.at(t1.pos);
                }

                if(s=="=="){
                    if(value==t2.val.at(t2.pos)){
                        k=1;
                    }else{
                        k=0;
                    }
                }else{
                    if(value!=t2.val.at(t2.pos)){
                        k=1;
                    }else{
                        k=0;
                    }
                }
               generateCodeFor_cmp_smaller_or_greater(t1,t2,s);   
            }    
            
            ////////////////////////////////////
        }
        bool check=t1.check_for_second_time;
        
        t2.reg=reg;
        if(check){
            if(t1.val.at(t1.pos)==k){
                t2.val.at(t2.pos)=k;
            }else{
                t2.val.at(t2.pos)=0;
            }
            t2.check_for_second_time=true;
        }else{
            t2.val.at(t2.pos)=k;
            t2.check_for_second_time=true;
        }
        t1=t2;
     }
     return t1;
}
void generateCodeFor_cmp_smaller_or_greater(exprVal &tok1 , exprVal &tok2 , string s){
     if(is_word(tok1.tok) || isKey(tok1.tok)){
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="<"){
                outfile<<"\tcmp<  r"<< reg << ",";
            }else if(s==">"){
                outfile<<"\tcmp>  r"<< reg << ",";
            }else if(s=="<="){
                outfile<<"\tcmp<=  r"<< reg << ",";
            }else{
                outfile<<"\tcmp>=  r"<<reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
        }else{
            if(s=="<"){
                outfile<<"\tcmp<  r"<<reg << ",";
            }else if(s==">"){
                outfile<<"\tcmp>  r"<< reg << ",";
            }else if(s=="<="){
                outfile<<"\tcmp<=  r"<< reg << ",";
            }else{
                outfile<<"\tcmp>=  r"<< reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
            //reg--;
        }   
    }else{
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="<"){
                outfile<<"\tcmp<  r"<< reg << ",";
            }else if(s==">"){
                outfile<<"\tcmp>  r"<< reg << ",";
            }else if(s=="<="){
                outfile<<"\tcmp<=  r"<< reg << ",";
            }else{
                outfile<<"\tcmp>=  r"<< reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" << tok2.reg <<endl;
        }else{
            if(s=="<"){
                outfile<<"\tcmp<  r"<< reg << ",";
            }else if(s==">"){
                outfile<<"\tcmp>  r"<< reg << ",";
            }else if(s=="<="){
                outfile<<"\tcmp<=  r"<< reg << ",";
            }else{
                outfile<<"\tcmp>=  r"<< reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
           
        }   
    }
}
exprVal add_expr(){
     exprVal t1=mul_expr();
     bool check_t1=true;
     bool check_t2=true;

     if(t1.type!=type_num && t1.type!=-1 &&  (T=="-"  ||  T=="+")){
            cout<<"line "<<line<<" : incompatible operands! + - \n";
            t1.type=-1;
            count_error++;
            check_t1=false;
     }
     if(t1.type==-1){
         check_t1=false;
     }
     while(myfile.good()  &&  (T=="-"  ||  T=="+")){
        string s=T;
        droptoken();
        T=gettoken(); 
        exprVal t2=mul_expr();
        check_t2=true;
        if(t2.type!=type_num && t2.type!=-1){
          cout<<"line : "<<line<<"   incompatible operands! + - \n";
          count_error++;
          check_t2=false;
        }
        if(t2.type==-1){
            check_t2=false;
        }
         if(check_t1 && check_t2){
               if(s=="+"){
                   t1.val.at(t1.pos)=t1.val.at(t1.pos)+t2.val.at(t2.pos);
               }else{
                   t1.val[t1.pos]=t1.val[t1.pos]-t2.val[t2.pos];
               }
             generateCodeForAddorSub(t1,t2,s);
             t1.tok=t1.val[t1.pos];
        }
     }
     return t1;
}
void generateCodeForAddorSub(exprVal &tok1 , exprVal &tok2 , string s){
    if(is_word(tok1.tok) || isKey(tok1.tok)){
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="+"){
                outfile<<"\tadd r"<< tok1.reg << ",";
            }else
            {
                outfile<<"\tsub r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
        }else{
            if(s=="+"){
                outfile<<"\tadd r"<< tok1.reg << ",";
            }else
            {
                outfile<<"\tsub r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
            reg--;
        }   
    }else{
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="+"){
                outfile<<"\tadd r"<< tok1.reg << ",";
            }else
            {
                outfile<<"\tsub r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" << tok2.reg <<endl;
        }else{
            if(s=="+"){
                outfile<<"\tadd r"<< tok1.reg << ",";
            }else
            {
                outfile<<"\tsub r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
            reg--;

        }   
    }
}

exprVal mul_expr(){
     exprVal t1=unary_expr();
     bool check_t1=true;
     bool check_t2=true;
     if(t1.type!=type_num  && t1.type!=-1 && (T=="*"  ||  T=="/"  ||  T=="%")){
        cout<<"line : "<<line<<"   incompatible operands! / * % \n";
        t1.type=-1;
        count_error++;
        check_t1=false;
     }
     if(t1.type==-1){
         check_t1=false;
     }
     while(myfile.good()  &&  (T=="*"  ||  T=="/"  ||  T=="%")){
        string s=T;
        droptoken();
        T=gettoken();
        exprVal t2=unary_expr();
        check_t2=true;
        if(t2.type!=type_num && t2.type!=-1){
          cout<<"line : "<<line<<"   incompatible operands! / * % \n";
          count_error++;
          check_t2=false;
        }
        if(t2.type==-1){
            check_t2=false;
        }
        if(check_t1 && check_t2){
            if(s=="*"){
                t1.val[t1.pos]=t1.val[t1.pos]*t2.val[t2.pos];
            }else if(s=="/"){
                t1.val[t1.pos]=t1.val[t1.pos]/t2.val[t2.pos];
            }else{
                t1.val[t1.pos]=remainder(t1.val[t1.pos],t2.val[t2.pos]);
            }
            generateCodeForMul_Div_Mod(t1,t2,s);
            t1.tok=t1.val[t1.pos];
        }
     }
     return t1;
}
void generateCodeForMul_Div_Mod(exprVal &tok1 , exprVal &tok2 , string s){
     if(is_word(tok1.tok) || isKey(tok1.tok)){
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="*"){
                outfile<<"\tmul r"<< tok1.reg << ",";
            }else if(s=="/"){
                outfile<<"\tdiv r"<< tok1.reg << ",";
            }else{
                outfile<<"\tmod r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
        }else{
            if(s=="*"){
                outfile<<"\tmul r"<< tok1.reg << ",";
            }else if(s=="/"){
                outfile<<"\tdiv r"<< tok1.reg << ",";
            }else{
                outfile<<"\tmod r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
            reg--;
        }   
    }else{
        if(is_word(tok2.tok) || isKey(tok2.tok)){
            if(s=="*"){
                outfile<<"\tmul r"<< tok1.reg << ",";
            }else if(s=="/"){
                outfile<<"\tdiv r"<< tok1.reg << ",";
            }else{
                outfile<<"\tmod r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" << tok2.reg <<endl;
        }else{
            if(s=="*"){
                outfile<<"\tmul r"<< tok1.reg << ",";
            }else if(s=="/"){
                outfile<<"\tdiv r"<< tok1.reg << ",";
            }else{
                outfile<<"\tmod r"<< tok1.reg << ",";
            }
            outfile<<" r"<< tok1.reg<< ", r" <<tok2.reg <<endl;
            reg--;
        }   
    }
}
exprVal unary_expr(){

     if(T=="!"  ||  T=="+"  ||  T=="-"){
         while(myfile.good()  &&  (T=="!"  ||  T=="+"  ||  T=="-")){
           droptoken();
           T=gettoken();  
           exprVal t1=postfix_expr();
           if(t1.type!=type_num && t1.type!=-1){
             t1.type=-1;
             cout<<"line "<<line<<" : incompatible operands! \n";
             count_error++;
           }  
           return t1;
        }
     }else{
         ////////////////////////////////////////////
         exprVal t2=postfix_expr();
         return t2;
     }
}
////l
exprVal postfix_expr(){
     exprVal t1=primary_expr();
     if(t1.type==type_num && T=="["){
           cout<<"line "<<line<<" : incompatible operands! name list\n";
           count_error++;
     }
     
     while(myfile.good()  &&  T=="["){
        droptoken();
        T=gettoken();
        exprVal t2= expr();
        if(t1.type!=type_num && t1.type!=-1 && t2.type==type_num ){
            reg++;            
            outfile<<"\tmov r"<<reg<<" , 1"<<endl;
            outfile<<"\tadd r"<<reg<<" , r"<<reg<<" , r"<<reg-1<<endl;
            reg++;
        }  

        if (T=="]") {

           t1.type=type_num;
           ///////////////////////////////
           int index2=search(t1.tok);
           t1.pos=t2.val[t2.pos];
           for(int k=t1.val.size();k<=t1.pos;k++){
               t1.val.push_back(0);
            
           }
      
           for(int k=symbolTable[index2].value.size();k<=t1.pos;k++){
               symbolTable[index2].value.push_back(0);
           
           }
           ////////////////////////////////
           outfile<<"\tmov r"<<reg<<" ,8"<<endl;
           outfile<<"\tmul r"<<reg<<" ,r"<<reg-1<<" ,r"<<reg<<endl;
           reg++;
           int index_i=search(t1.tok);

           if(index_i!=-1){
               outfile<<"\tadd r"<<symbolTable[index_i].reg<<" ,r"<<symbolTable[index_i].reg<<" ,r"<<reg-1<<endl;
               outfile<<"\tmov r"<<reg<<" ,r"<<symbolTable[index_i].reg<<endl;
        
               outfile<<"\tsub r"<<symbolTable[index_i].reg<<" ,r"<<symbolTable[index_i].reg<<" ,r"<<reg-1<<endl;
               t1.reg=reg;
               if(myfile.good()){
                    droptoken();
                    T=gettoken();
                    if(T!="="){
                        reg++;
                        outfile<<"\tld r"<<reg<<" ,r"<<t1.reg<<endl;
                        t1.reg=reg;
                    }
                }
          
           }else if(myfile.good()){
                droptoken();
                T=gettoken();
            }
           return t1;
        }else{
           cout<<"line "<<line<<" : error  ] \n";
           count_error++;
           droptoken(); 
           T=gettoken();
           exprVal exp;
           exp.type=-1;
           return exp;
        }
     }
     return t1;

}
///l
exprVal primary_expr(){

      if(is_word(T)){
        if(myfile.good()){
            droptoken();
            string T1=T;
            T=gettoken();
            if(T=="("){
                bool isfunc=searchIsFunc(T1 );
                int index;
                index = searchIsFunc_index(T1);
                if(isfunc){
                    if(myfile.good()){
                        droptoken();
                        T=gettoken();

                        int  callFunctionsParams=0;
                        int lastReg=-1;
                        int NUM2=-1;
                        if(T!=")"){
 
                            if(!numprint(T1) && !MakeList(T1)){
                
                            }else if(numread(T1)){
                                lastReg=2;
                             }

                            bool first_param=false;
                            int last_parametr;
                            callFunctionsParams=clist(index,callFunctionsParams ,lastReg,first_param,NUM2,T1 ,last_parametr);

                            if(!numprint(T1) && !MakeList(T1)){
                                outfile<<" ,r"<<last_parametr;
                                outfile<<endl;
                            }else if(numread(T1)){
                                symbolTable[index].reg=lastReg;
                            }
                        }else if(!numread(T1)){
                            outfile<<"\tcall "<<T1<<endl;
                            reg++;
                            symbolTable[index].reg=reg; 
                            lastReg=reg;
                            exprVal exp;
                            exp.tok="";
                            exp.reg=reg;
                            exp.val.push_back(0);
                            exp.type=type_num;
                            if(myfile.good()){
                                droptoken();
                                T=gettoken();
                            }
                            return exp;
                        }

                        if(numread(T1)){
                          
                            symbolTable[index].reg=lastReg;
                      
                        }else if(numprint(T1)){
                            outfile<<"\tcall iput , r"<<lastReg<<endl;
                        }else if(MakeList(T1)){
                        
                            symbolTable[index].reg=lastReg;
                            symbolTable[index].len=NUM2;
                  
                        }else{
                            symbolTable[index].reg=lastReg;   
                        }
                        if(callFunctionsParams!=symbolTable[index].numOfParams ){
                           cout<<"line "<<line<<" : number of parametrs is not same as "<<T1<<"  function \n";        
                           count_error++;
                        }
                        if(myfile.good()){
                           droptoken();
                           T=gettoken();
                        }
                    }
                    exprVal exp;
                    exp.tok=T1;
        
                    exp.type=symbolTable[index].type;
                    if(numread(T1)){
                          
                        exp.val.push_back(0);
                      
                    }else{
                        exp.len=symbolTable[index].len;
                        exp.reg=symbolTable[index].reg;
                        exp.val.push_back(0);
                      
                    }
               
                    return exp;
                }else{
                    cout<<"line "<<line<<" : function  "<< T1<<"  is not declared \n";
                    count_error++;
                    if(myfile.good()){
                        droptoken();
                        T=gettoken();

                        int callFunctionsParams=0;
                        
                        if(T!=")"){
                            int d;
                            bool f=false;
                            int o=-1;
                            
                            callFunctionsParams=clist(-1, callFunctionsParams ,d , f ,o,T1 ,o);
                        }
                        if(myfile.good()){
                           droptoken();
                           T=gettoken();
                        }
                    }
                    exprVal exp;
                    exp.type=-1;
                    return exp;
                }
            }else{
                bool exist=searchExistIden(T1);
                if(!exist){
                    cout<<"line "<<line<<" : identifier  "<< T1<<"  is not declared \n";
                    count_error++;
                    exprVal exp;
                    exp.type=-1;
                    exp.tok=T1;
                    return exp;
                }  
            
                exprVal exp;
                if(!symbolTable[index_iden].R_Reg){
                    reg++;
                    exp.tok=T1;
                    exp.reg=reg;
                    exp.type=symbolTable[index_iden].type;
                    symbolTable[index_iden].R_Reg=true;
                    symbolTable[index_iden].reg=reg;
                    ////////////////////////////////////////////////
                    reg++;
                    if(exp.type==type_num){
                        exp.val.push_back(0);
                   
                        symbolTable[index_iden].value=exp.val;
                    }else{
                        //////////////////////////////
                    }
         
                    ///////////////////////////////////////////////
                }else{
                    exp.tok=T1;
                    exp.reg=symbolTable[index_iden].reg;
                    exp.type=symbolTable[index_iden].type;
                    ////////////////////////////////////////////////
                    if(exp.type==type_num){
                        exp.val=symbolTable[index_iden].value;
                    }else{
                        exp.val=symbolTable[index_iden].value;
                    }

                    ///////////////////////////////////////////////
                }
                return exp;
            }
        }
    }else if(is_number(T)){ 
        int T2=stoi(T);
        string TT2=T;
        exprVal exp;
        reg++;
        exp.tok=TT2;
        exp.type=type_num;
        exp.val.push_back(T2);
        outfile<<"\tmov r"<<reg<<" ,"<<T2<<endl;
        exp.reg=reg;
        
        if(myfile.good()){
                droptoken();
                T=gettoken();
        }
        return exp;
    }else if(T=="("){
         if(myfile.good()){
             droptoken();
             T=gettoken();
             ////////////////////////////////////////////
             exprVal t1=expr();
             ///////////////////////////////////////////
             if(T!=")"){
                 cout<<"line "<<line <<" : error )\n";
                 count_error++;
             }
             if(myfile.good()){
                droptoken();
                T=gettoken();
             }
             return t1;
         }
    }else{
         cout<<"line "<<line <<" : syntax error\n";
         count_error++;
         if(myfile.good()){
                droptoken();
                T=gettoken();
         }
   
    }
}

////flist
int flist(vector<table> &LISTT){

    if(T!=")"){
        int T_type=Type(T);
        if(T_type!=type_num && T_type!=type_list){
            cout<<"line "<<line <<" : type  "<<T<<" not exist \n";
            count_error++;
        }
        if(myfile.good()){
            droptoken();
            T=gettoken();
    
            if(!is_word(T)){
                cout<<"line "<<line <<" : identifire name sholude be word  \n";
                count_error++;
            }
            if(isKey(T)){
               cout<<"line "<<line<<" :  identifire name sholude'nt be key value\n";
               count_error++;
            }

                nparams++;
                table s;
                s.type=T_type;
                s.isFunc=false;
                s.var=T;
                s.R_Reg=true;
                s.reg=reg;
                s.numOfParams=0;
                s.value.push_back(0);
                /////////////////////////////
                LISTT.push_back(s);
                ////////////////////////////
                if(myfile.good()){
                  droptoken();
                  T=gettoken();
                    if(T==","){
                        if(myfile.good()){
                          droptoken();
                          T=gettoken();
                          reg++;
                          nparams=flist(LISTT);
                        }
                    }else if(T==")"){
                        nparams=flist(LISTT);
                  
                    }else{
                        cout<<"line "<<line <<" : error  , \n";
                        count_error++;
                        if(myfile.good()){
                          droptoken();
                          T=gettoken();
                          nparams=flist(LISTT);
                        }
                    }
                }
        }
    }
    return nparams;
}

//clist
int clist(int indexFunc , int countp , int &lReg , bool &first_p , int &N ,string func_name, int &last_param){
        countp++;
        string TT=T;
        
        exprVal t1=expr();

        if(lReg==-1){
            int x=search(t1.tok);
            if(t1.type==type_num && symbolTable[x].type==type_list ){
                lReg=reg;
            }else if(!first_p){
                first_p=true;
                lReg=t1.reg;
                N=t1.val.at(t1.pos);
            }
        }
        if(indexFunc!=-1 && t1.type!=-1){
             if(t1.type!=symbolTable[indexFunc].list[countp-1].type){
                 cout<<"line "<<line<<" : type of input params of function is not same as function you call "<<endl;
                 count_error++;
             }
        }
        if(T==","){
            if(myfile.good()){
                droptoken();
                T=gettoken();
                int NUM1=-1;
             
                countp=clist(indexFunc , countp,lReg,first_p , N,func_name , last_param);
           
                if(!numprint(symbolTable[indexFunc].var) && !MakeList(symbolTable[indexFunc].var)){
                    outfile<<" ,r"<<t1.reg;
                }
            }
        }else if(T==")"){
           
            last_param=t1.reg;
          
            if(!numprint(symbolTable[indexFunc].var) && !MakeList(symbolTable[indexFunc].var)){
                    outfile<<"\tcall "<<func_name;
            }
            
            return countp;
        }else{
            cout<<"line "<<line<<" : clist error )\n";
            count_error++;
            T=gettoken();
            droptoken();
            return countp;
        }
       
    return countp;
}

//Type
int Type(string c){
    if(c=="num"){
        return 1;
    }else if(c=="list"){
        return 2;
    }
  
    return -1;
}

//Type function
int Type_function(string c){
    if(c=="num"){
        return 1;
    }else if(c=="list"){
        return 2;
    }else if(c=="nil"){
        return 3;
    }
 
    return -1;
}

// iden // word
bool is_word(string w){
    if(w.size()==0) return false;
    if(w[0]=='_' || ( (int)w[0]>=65 && (int)w[0]<=90 ) || ((int)w[0]>=97 && (int)w[0]<=122) ){
        for(int i=1;i<w.size();i++) {
            if((w[i]=='_' || ( (int)w[i]>=65 && (int)w[i]<=90 ) || ((int)w[i]>=97 && (int)w[i]<=122) || ((int)w[i]>=48 && (int)w[i]<=57) )==false)  {
                return false ;
            }
        }
    }else{
        return false;
    }
    return true;
}

int searchIsFunc_index(string T){
    for( int i=0 ; i<symbolTable.size() ; i++){
         if(symbolTable[i].isFunc){
             if(symbolTable[i].var==T){
                 return i;
             }
         }
     }
     return -1;
}
//search
bool searchIsFunc(string T){
     for( int i=0 ; i<symbolTable.size() ; i++){
         if(symbolTable[i].isFunc){
             if(symbolTable[i].var==T){
                 return true;
             }
         }
     }
     /////////////////////////////////////////////
     return isKey(T);
}

bool searchExistIden(string T){
     for( int i=0 ; i<symbolTable.size() ; i++){
         if(!symbolTable[i].isFunc){
             if(symbolTable[i].var==T){
                 index_iden=i;
                 return true;
             }
         }
     }
     /////////////////////////////////
     return isKey(T);
}
int searchIndexIden(string T){
    for( int i=0 ; i<symbolTable.size() ; i++){
        if(symbolTable[i].var==T){
            return i;
        }
    }
    return -1;
}
int search(string y){
    for( int i=0 ; i<symbolTable.size() ; i++){
        if(symbolTable[i].var==y){
            return i;
        }
     }
     return -1;
}
bool Main(string word){
    if (word=="main") {
        return true;
    }
    return false;
}

bool IF(string word){
    if (word=="if") {
        return true;
    }
    return false;
}
bool While(string word){
    if (word=="while") {
        return true;
    }
    return false;
}
bool For(string word){
    if (word=="for") {
        return true;
    }
    return false;
}
bool Else(string word){
    if (word=="else") {
        return true;
    }
    return false;
}
bool In(string word){
    if (word=="in") {
        return true;
    }
    return false;
}
bool Num(string word){
    if (word=="num") {
        return true;
    }
    return false;
}
bool Return(string word){
    if (word=="return") {
        return true;
    }
    return false;
}
bool numread(string word){
    if (word=="numread") {
        return true;
    }
    return false;
}
bool numprint(string word){
    if (word=="numprint") {
        return true;
    }
    return false;
}
bool MakeList(string word){
    if (word=="makelist") {
        return true;
    }
    return false;
}
bool Listlen(string word){
    if (word=="listlen") {
        return true;
    }
    return false;
}
bool Exit(string word){
    if (word=="exit") {
        return true;
    }
    return false;
}
bool isKey(string word){
    return Main(word) || IF(word) || While(word) || For(word) || Else(word) || In(word)
        || Num(word) || Return(word) || numread(word) || numprint(word) || MakeList(word)
        || Listlen(word) || Exit(word);
}

/// is_identifier_char
bool is_identifier_char(char ch){
    int asci = (int)ch;
    if( (ch == '_') || (asci>=65 && asci<=90) || (asci>=97 && asci<=122) ){
        return true;
    }else{
        return false;
    }
}

char getcharr(){
    char c1;
    c1=myfile.get();
    return c1;
}

void ungetcharr(){
    myfile.unget();
}

///nextToken
string NextToken(){
    char c;
    if(myfile.good()){
       c=getcharr();
    }

    while( (c==' ' || c=='\n' || c=='\r') && myfile.good()){
        if(c=='\n'){
            line++;
        }
        c=getcharr();
    }
    
    /// comments
    if(c=='#'){
        string g;
        getline(myfile,g);
        if(myfile.good()){
            c=getcharr();
        }else{
            return "";
        }
    }
    ////////////////
    /////////////////////////////////////////////////
    if(c==':'){
       if(myfile.good()){
            c=getcharr();
            if(c=='='){
                return ":=";
            }
            ungetcharr();
            return ":";
        }    
    }else if(c=='^'){
       if(myfile.good()){
           c=getcharr();
           if(c==';'){
               return "^;";
           }
           ungetcharr();
           return "^";
       }
       return "^" ;
    }else if(c=='~'){
       return "~";
    }else if(c==';'){
        return ";";
    }else if(c==','){
        return ",";
    }else if(c=='('){
        return "(";
    }else if(c==')'){
        return ")";
    }else if(c=='{'){
        return "{";
    }else if(c=='}'){
        return "}";
    }else if(c=='['){
        return "[";
    }else if(c==']'){
        return "]";
    }else if(c=='|'){
        if(myfile.good()){
            c=getcharr();
            if(c=='|'){
                return "||";
            }else{
                ungetcharr();
                /////////////////////////////////////////////////////////////
                //return "|";
            }
        }    
        return "";
    }else if(c=='&'){
        if(myfile.good()){
            c=getcharr();
            if(c=='&'){
                return "&&";
            }
        }
        ungetcharr();
        /////////////////////////////////////////////////////////////////////
        //return "&";
    }else if(c=='!'){
        if(myfile.good()){
            c=getcharr();
            if(c=='='){
                return "!=";
            }
        }    
        ungetcharr();
        return "!";
    }else if(c=='<'){
        if(myfile.good()){
            c=getcharr();
            if(c=='='){
                return "<=";
            }
        }
        ungetcharr();
        return "<";
    }else if(c=='>'){
        if(myfile.good()){
            c=getcharr();
            if(c=='='){
                return ">=";
            }
        }    
        ungetcharr();
        return ">";
    }else if(c=='='){
        if(myfile.good()){
            c=getcharr();
            if(c=='='){
                return "==";
            }
        }    
        ungetcharr();
        return "=";    
    }else if(c=='+'){
        return "+";    
    }else if(c=='-'){
        return "-";
    }else if(c=='*'){
        return "*";
    }else if(c=='/'){
        return "/";
    }else if(c=='%'){
        return "%";
    }else if(isdigit(c) && myfile.good()){        
        
        string tok="";
        tok += c;
        if(myfile.good()){    
            c=getcharr();
            while( (isdigit(c) || is_identifier_char(c) || c=='.' )  && myfile.good()){
                 tok += c;
                c=getcharr();
            }
            ungetcharr();
        }
        return tok;
    }else if(is_identifier_char(c) && myfile.good()){
         
         string tok="";
         tok+=c;
         if(myfile.good()){ 
             c=getcharr();
             while((is_identifier_char(c)  ||  isdigit(c) || c=='.') && myfile.good()){
                 tok+=c;
                 c=getcharr();
             } 
             ungetcharr();
         } 
         return tok;
    }
   return "";
}

///signes
bool is_signes(string s){
    if(s.size()==0) return false;
    if(s.size()==1 && ( s==" " ||  s=="\n"  || s=="\t" || s=="\r")) return false ;
    if(s==":=" || s=="^;" || s=="|") return false;
    for(int i=0;i<s.size();i++) {
         if( (s[i]>='0' && s[i]<='9') || ((int)s[i]>=65 && (int)s[i]<=90 ) || ((int)s[i]>=97 && (int)s[i]<=122) ){
             return false;
         }
    }
    return true;
} 

// number
bool is_number(string s) {
    if(s.size()==0) return false;
    for(int i=0;i<s.size();i++) {
        if((s[i]>='0' && s[i]<='9')==false) {
            return false;
        }
    }
    return true;
}