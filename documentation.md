# Arduino audio
## 1. Hardware platform
1. ATmega 328p
Много разпространен микроконтролер с добра поддръжка от различни библиотеки. Той е много евтин и поддържа добро разнообразие на комуникационни протоколи, особено за цената си. Също така има много налична информация за работата му, което го прави удобен за разработка. Проектът използва цялостната платка на Arduino (uno или nano), но всички разновидности с този микроконтролер са съвместими.
Микроконтролерът използва модифицирана Харвард архитектура - avr. При нея липсват много екстри като модули за управление на паметта, защтото има малка нужда от тях.
Важни негови характеристики, използвани в проекта:
    * 16 MHz честота - сравнително високата честота позволява достатъчно изчислителна мощ за възпроизвеждане на звук, макар и с ниско качество.
    * Pulse Width Modulation (PWM) - 8 битова модулация с опция за правилна фаза, задвижван от осцилатора на микроконтролера. Макар че той няма цифрово-аналогов преобразувател, неговата функционалност може да се имитира с PWM и прост филтър.
    * UART комуникация - бърз и прост сериен интерфейс, идеален за лесно предаване на сравнително голям обем на иформация. В платките на Arduino един от тях може да се използва през USB порта.

1. Сензори
Комуникация с външния свят почти няма, заящото не е необходима. Въпреки това е включен бутон за пускане/спиране на възпроизваженето на звук.

1. Актуатори
Има само един - говорителят. Той е свързан на PWM пина след low-pass филтър за преобразуване на сигнала в такъв, който е подходящ за възпроизвеждане на звук. По-голям говорител може да бъде свързан с усилвател.

## 2. Software platform
1. avr-libc
Проектът няма операционна система. Управляваме хардуера директно, защото е необходима голяма прецизност за възпроизвеждането на звук без изкривявания, тоест не използваме дори и библиотеката на Arduino. Библиотеката `avr-libc` ни предоставя дефиниции на регистри, вектори за прекъсвания и малък набор от помощни функции за улесняване на работата. Проектът се опира на avr-gcc и avrdude за компилация и качване на финалната програма. За улеснение на процеса използваме системата make, която автоматизира много от действията.

1. python
За комуницаия с микроконтролера използваме python, защото е лесен за употреба, а компютъра, на който се изпълнява е предостатъчно бърз, за да не се притесняваме от това. За улеснение на UART комуникацията използваме библиотеката `pyserial`.

## 3. SDD - Static view
   Състои се от 6 компонента: Decoder, SD writer, SD card, SD reader, GPIO Driver и Low-pass Filter.
Декодера стои на периферно включен компютър, който прочита wav файл и изпраща некомпресиран аудио поток до ардуиното по UART. Потокът се приема от ардуиното и се записва на SD картата през SPI. При сигнал за пускане, от бутон, започва обратно да се чете от SD картата и информацията се записва на равни интервали на регистъра за PWM, който управлява GPIO пиновете. Накрая нискочестотният филтър премахва високочестотните компоненти от правоъгълния сигнал.

```
+-------------+             +-------------+       +-------+         +------------+           +------+
|   Decoder   | ---UART---> |  SD writer  |--SPI--|SD card|--SPI--> |  SD reader |---pwm---> | GPIO |
+-------------+             +-------------+       +-------+         +------------+           +------+
                                                                                               |
                                                                                               |
                                                                                               |
                                                                                               V
                                                                                     +-------------------+
                                                                                     |  Low-pass filter  |
                                                                                     +-------------------+
```

## 4. SDD - Dynamic View
   След инициализация на усторйството се чака от външен вход. Първо декодера инциира изпращане на информация към ардуиното. Той разчита wav формата и преобразува информацията, ако е необходимо. След това се

При изпращенето на нова информация по UART се създава прекъсване, което записва новите данни в Swap Buffer. След записването, буфера съобщава на декодера(Decoder) че нова информация е пристигнала. На декодера се дава адресът на записаната информация, докато буфере получава нов, за да може при пирстигането на нов inerrupt данните да не се презапишат върху още необработените такива. След като декодера пригоди сигнала в годен за изпращане по PWM, данните се възпроизвеждат.
```
   Decoder             SD writer           SD card            SD reader       PWM register
    |                     |                   |                   |                |
 decode---sample_ready--->|                   |                   |                |
    |<----acknowledge-----|---write_sample--->|                   |                |
    |                     |<----ready---------|                   |                |
    |                     |                 play---read_sample--->|---set_pwm----->|
    |                     |                   |<------ready-------|                |
    |                     |                   |                   |                |
    ...
```

