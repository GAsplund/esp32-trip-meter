#ifndef ELM327_H_
#define ELM327_H_

String converttohex (int a, int decimals);
void setupELM327();
void ELM327_poll();

#endif // ELM327_H_