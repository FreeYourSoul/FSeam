//
// Created by FyS on 4/3/19.
//

#ifndef FSEAM_FREEFUNCTIONCLASS_HH
#define FSEAM_FREEFUNCTIONCLASS_HH


namespace source {

    void freeFunctionSimple();

    int freeFunctionReturn();

    void freeFunctionWithArguments(int arg1, int arg2, char arg3);

    class FreeFunctionClass {

    public:
        static int staticFunction();
    };

}


#endif //FSEAM_FREEFUNCTIONCLASS_HH
