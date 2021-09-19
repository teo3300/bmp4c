#pragma once

#define _NOP_

#ifdef Debug
    #define PRINT(file, msg)        fprintf(file, "%3d (%10s): %s\n",      __LINE__, __func__, msg)
    #define PRINTSTR(msg)           fprintf(stdout, "%3d (%10s): \"%s\" -> \"%s\"\n",  __LINE__, __func__, #msg, msg)
    #define PRINTVAL(file, labl)    fprintf(file, "%3d (%10s): \"%s\" -> %d\n", __LINE__, __func__, #labl, labl)
    #define PRINTPTR(file, labl)    fprintf(file, "%3d (%10s): \"%s\" -> %p\n", __LINE__, __func__, #labl, labl)
    #define DEBUG(labl)             PRINTVAL(stdout, labl)
    #define DEPTR(labl)             PRINTPTR(stdout, labl)
    #define LOG(msg)                PRINT(stdout, #msg)
    #define ERR(msg)                PRINT(stderr, #msg)
#endif
#ifndef Debug
    #define PRINT(msg, file)        _NOP_
    #define PRINTSTR(msg)           _NOP_
    #define PRINTVAL(file, labl)    _NOP_
    #define PRINTPTR(file, labl)    _NOP_
    #define DEBUG(labl)             _NOP_
    #define DEPTR(labl)             _NOP_
    #define LOG(msg)                _NOP_
    #define ERR(msg)                _NOP_
#endif

// Utilizzo:
//  LOG(stringa da stampare su stdout senza apici);
//  ERR(stringa da stampare su stderr senza apici);
//  DEBUG(variabile di cui stampare il contenuto);

//ex:
//    (comando) --> (su stdout)
//    (comando) ++> (su stderr)
//  LOG(stringa di messaggio);  --> "<numero riga>: stringa di messaggio\n"
//  ERR(stringa di errore);     ++> "<numero riga>: stringa di errore\n"
//  DEBUG(variabile);           --> "<numero riga>: "nome_variabile" -> valore_variabile\n"   (solo con int)