Projekt MIT zimni

Funkce projektu:
Projekt funguje jako klasické minutky, tedy odpočítává nastavený čas a poté vydá určitou signalizaci.
Enkodérem se nastaví (otáčení doprava = + ,doleva = -)  požadovaný čas minutek (až 99 minut a 99 sekund) a stiskem se to potvrdí.) 
Tím se přepnou minutky do odpočítávajícího režimu a začnou po sekundách odpočítávat daný čas.
Jakmile čas doběhne na nulu, rozezní se repráček (pípá) a začne blikat ledka, dokud se zase nestiskne tlačítko.
Mezi tím je čas neustále zobrazován na displeji.

Obvod mám zapojený v nepájivém poli a skládá se ze: 
1x STM8 kit; 
1x LCD displej (QAPASS 1602A); 
1x Rotační enkodér (HW-040);
1x Potenciometr (5k-50k); 
1x Červená LED dioda; 
2x Rezistor (k55); 
1x Piezoelektrický reproduktor (murata 01u) 
a spousta drátků.
Obvod je napájen +5V přes mikro USB z PC.

![Jednoduché blokové schéma]( https://github.com/mikroprocesorovka/1-projekt-zimni-TurboMaster69/blob/main/schema%20proj1.png )
