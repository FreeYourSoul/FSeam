//
// Created by FyS on 4/3/19.
//

#ifndef FSEAM_CLASSWITHCONSTRUCTOR_HH
#define FSEAM_CLASSWITHCONSTRUCTOR_HH


namespace source {

    class ClassWithConstructor {

    public:
        ~ClassWithConstructor();
        ClassWithConstructor();

        int mockedMethod() const;
    };

}


#endif //FSEAM_CLASSWITHCONSTRUCTOR_HH
