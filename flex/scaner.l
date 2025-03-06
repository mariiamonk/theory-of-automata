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
(?i:[a-z][a-z0-9]{0,19})\.(?i:[a-z][a-z0-9]{0,19})\./(?i:[a-z]{1,5}\n)  {BEGIN(ZONE);}
\n            {BEGIN(INITIAL); return 0;}
.             {BEGIN(INITIAL);}
}

<ZONE>{
(?i:[a-z]){1,5}\n {return 1;}
\n            {BEGIN(INITIAL);}
}

%%
