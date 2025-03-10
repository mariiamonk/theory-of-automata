// #include "AppClass.h"

//     void AppClass::addSymbol(char symbol){ buffer += symbol;}

//     bool AppClass::isCorrectCharSymbol(char symbol){
//         return ((symbol >= 'A' && symbol <= 'Z') || (symbol >= 'a' && symbol <= 'z'));
//     }

//     bool AppClass::isCorrectNumSymbol(char symbol){
//         return (symbol >= '0' && symbol <= '9');
//     }

//     bool AppClass::isCorrectNameSymbol(char symbol){
//         return isCorrectCharSymbol(symbol) || isCorrectNumSymbol(symbol);
//     }

//     bool AppClass::lenghNameInput(){ return buffer.size() < 20;}
//     bool AppClass::lenghZoneInput(){ return buffer.size() < 5;}

//     bool AppClass::isHTTPorWWW(){ return buffer == "http://" || buffer == "www.";}

//     void AppClass::resetAutomata(){ _fsm.Reset(); firstPartofName = false; isAcceptable = false;}

//     std::string AppClass::checkString(std::string const& string){
//         for (auto i: string){
//             switch (i) {
//                 case '.':
//                     _fsm.Dot(); break;
//                 default :
//                     _fsm.Symbol(i); break;
//             }
//         }
//         _fsm.EOS();
    
//         if(isAcceptable) {
//             return (buffer);
//         }
//     }