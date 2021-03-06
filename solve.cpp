#include "solve.h"

static Slvs_System sys;

static void *CheckMalloc(size_t n)
{
    void *r = malloc(n);
    if(!r) {
        printf("out of memory!\n");
        exit(-1);
    }
    return r;
}

std::array<double, 6> ChooseMethodAndRun(const char* method, const char* arg1, const char* arg2, const char* arg3) {
    std::array<double, 6> res = {0, 0, 0, 0, 0, 0};
    int num = atoi(method);
    if (num == 1)
    res = solve1(atof(arg1), atof(arg2), atof(arg3));
    else if (num == 2)
    res = solve2(atof(arg1), atof(arg2), atof(arg3));
    else if (num == 3)
    res = solve3(atof(arg1), atof(arg2), atof(arg3));
    return res;
};

void start() {
    memset(&sys, 0, sizeof(sys));
    sys.param      = static_cast<Slvs_Param *>(CheckMalloc(50 * sizeof(sys.param[0])));
    sys.entity     = static_cast<Slvs_Entity *>(CheckMalloc(50 * sizeof(sys.entity[0])));
    sys.constraint = static_cast<Slvs_Constraint *>(CheckMalloc(50 * sizeof(sys.constraint[0])));
    sys.failed  = static_cast<Slvs_hConstraint *>(CheckMalloc(50 * sizeof(sys.failed[0])));
    sys.faileds = 50;

    int g;
    g = 1;
    double qw = 0, qx = 0, qy = 0, qz = 0;

    sys.param[sys.params++] = Slvs_MakeParam(1, g, 0.0);
    sys.param[sys.params++] = Slvs_MakeParam(2, g, 0.0);
    sys.param[sys.params++] = Slvs_MakeParam(3, g, 0.0);
    sys.entity[sys.entities++] = Slvs_MakePoint3d(101, g, 1, 2, 3);

    Slvs_MakeQuaternion(1, 0, 0,
                        0, 1, 0, &qw, &qx, &qy, &qz);
    sys.param[sys.params++] = Slvs_MakeParam(4, g, qw);
    sys.param[sys.params++] = Slvs_MakeParam(5, g, qx);
    sys.param[sys.params++] = Slvs_MakeParam(6, g, qy);
    sys.param[sys.params++] = Slvs_MakeParam(7, g, qz);
    sys.entity[sys.entities++] = Slvs_MakeNormal3d(102, g, 4, 5, 6, 7);
    sys.entity[sys.entities++] = Slvs_MakeWorkplane(200, g, 101, 102);

    g = 2;

    sys.param[sys.params++] = Slvs_MakeParam(11, g, 6);
    sys.param[sys.params++] = Slvs_MakeParam(12, g, 7);
    sys.entity[sys.entities++] = Slvs_MakePoint2d(301, g, 200, 11, 12);

    sys.param[sys.params++] = Slvs_MakeParam(13, g, 2.5);
    sys.param[sys.params++] = Slvs_MakeParam(14, g, -8);
    sys.entity[sys.entities++] = Slvs_MakePoint2d(302, g, 200, 13, 14);

    sys.param[sys.params++] = Slvs_MakeParam(15, g, -9);
    sys.param[sys.params++] = Slvs_MakeParam(16, g, 2);
    sys.entity[sys.entities++] = Slvs_MakePoint2d(303, g, 200, 15, 16);

    sys.entity[sys.entities++] = Slvs_MakeLineSegment(400, g, 200, 301, 302);
    sys.entity[sys.entities++] = Slvs_MakeLineSegment(401, g, 200, 301, 303);
    sys.entity[sys.entities++] = Slvs_MakeLineSegment(402, g, 200, 302, 303);

}

std::array<double, 6> finish() {
    std::array<double, 6> res = {0, 0, 0, 0, 0, 0};
    if(sys.result == SLVS_RESULT_OKAY) {
        for (int i = 7; i <= 12; i++) {
            res[i - 7] = sys.param[i].val;
        }
        sys.params = sys.constraints = sys.entities = 0;
        return res;
    } else {
        sys.params = sys.constraints = sys.entities = 0;
        return res;
        }
}

std::array<double, 6> solve1(double l1, double l2, double a) {
    start();
    if (a >= 180) {
        sys.result = 2;
        return finish();
    }
    int g = 2;
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            1, g,
            SLVS_C_ANGLE,
            200,
            a,
            0, 0, 400, 401);
    Slvs_Solve(&sys, 2);
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            2, g,
            SLVS_C_PT_PT_DISTANCE,
            200,
            l1,
            301, 302, 0, 0);
    Slvs_Solve(&sys, 2);
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            3, g,
            SLVS_C_PT_PT_DISTANCE,
            200,
            l2,
            301, 303, 0, 0);
    Slvs_Solve(&sys, 2);
    return finish();
}

std::array<double, 6> solve2(double l, double a1, double a2) {
    start();
    if ( a1 + a2 >= 180){
        sys.result = 2;
        return finish();
    }
    if ( std::abs(a1 - a2) < 0.001 && a1 <= 60.001 && a1 >= 59.99) {
	    a1 = 120;
    } 
    if (a1 > a2)
        std::swap(a1, a2);
    /*if (a1 == a2)
        a2 += 0.1;*/
    int g = 2;
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            1, g,
            SLVS_C_ANGLE,
            200,
            a1,
            0, 0, 400, 401);
    Slvs_Solve(&sys, 2);
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            2, g,
            SLVS_C_ANGLE,
            200,
            a2,
            0, 0, 400, 402);
    Slvs_Solve(&sys, 2);
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            3, g,
            SLVS_C_PT_PT_DISTANCE,
            200,
            l,
            301, 302, 0, 0);
    Slvs_Solve(&sys, 2);
    return finish();
}

std::array<double, 6> solve3(double l1, double l2, double l3) {
    start();
    if ( l1 + l2 < l3 or l1 + l3 < l2 or l2 + l3 < l1 or l1 == 0 or l2 == 0 or l3 == 0) {
        sys.result = 2;
        return finish();
    }
    int g = 2;
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            1, g,
            SLVS_C_PT_PT_DISTANCE,
            200,
            l1,
            301, 302, 0, 0);
    Slvs_Solve(&sys, 2);
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            2, g,
            SLVS_C_PT_PT_DISTANCE,
            200,
            l2,
            301, 303, 0, 0);
    Slvs_Solve(&sys, 2);
    sys.constraint[sys.constraints++] = Slvs_MakeConstraint(
            3, g,
            SLVS_C_PT_PT_DISTANCE,
            200,
            l3,
            302, 303, 0, 0);
    Slvs_Solve(&sys, 2);
    return finish();
}

bool CheckError(const std::array<double, 6> &res) {
    if(std::all_of(res.begin(), res.end(), [](double num){return num != 0;}))
        return true;
    else
        return false;
}

std::string GenerateSlvs(const std::array<double,6> &res, const char* name) {
    std::string newname = name;
    newname = "../" + newname + ".slvs";

    std::array<std::string, 6> newres = {};
    for (int i = 0; i < 6; i++) {
        newres[i] = std::to_string(res[i]);
    }

    std::string line;
    std::array<char[8], 6> str = {"1.11111", "2.22222", "3.33333", "4.44444", "5.55555", "6.66666"};

    std::ifstream in("../template.slvs");
    std::ofstream out(newname);
    std::string newin = "../newin.slvs";
    for (int i = 0; i < 6; i++) {
        while (getline(in, line)) {
            while (true) {
                size_t pos = line.find(str[i]);
                if (pos != std::string::npos)
                    line.replace(pos, 8, newres[i]);
                else
                    break;
            }
            out << line << '\n';
        }
        in.close();
        out.close();
        std::filesystem::remove(newin);
        std::filesystem::copy_file(newname, newin);
        std::filesystem::remove(newname);
        in.open(newin);
        out.open(newname);
    }
    std::filesystem::rename(newin, newname);
    return newname;
}