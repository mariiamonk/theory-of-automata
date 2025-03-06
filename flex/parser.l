%option C++ noyywrap
%x SERV_DOM ZONE

%%

<INITIAL>{
^http\:\/\/      {BEGIN(SERV_DOM);}
^www\.        {BEGIN(SERV_DOM);}
\n            { return 0; }
.             ; 
}

<SERV_DOM>{
[a-zA-Z0-9]{1,20}\.[a-zA-Z0-9]{1,20}\./([a-zA-Z]{1,5}$)  {BEGIN(ZONE);}
\n            {BEGIN(INITIAL);}
.             {BEGIN(INITIAL);}
}

<ZONE>{
[a-zA-Z]{1,5}$  {BEGIN(INITIAL); return 1;}
\n            {BEGIN(INITIAL);}
.             
}

%%
