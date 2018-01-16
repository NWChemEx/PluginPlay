#include <NWChemExRuntime/AtomicInfo.hpp>
#include <catch/catch.hpp>

using namespace NWXRuntime::detail_;
TEST_CASE("Symbol to Z conversion")
{
    REQUIRE(sym2Z_.at("gho") == 0);
    REQUIRE(sym2Z_.at("h") == 1);
    REQUIRE(sym2Z_.at("he") == 2);
    REQUIRE(sym2Z_.at("li") == 3);
    REQUIRE(sym2Z_.at("be") == 4);
    REQUIRE(sym2Z_.at("b") == 5);
    REQUIRE(sym2Z_.at("c") == 6);
    REQUIRE(sym2Z_.at("n") == 7);
    REQUIRE(sym2Z_.at("o") == 8);
    REQUIRE(sym2Z_.at("f") == 9);
    REQUIRE(sym2Z_.at("ne") == 10);
    REQUIRE(sym2Z_.at("na") == 11);
    REQUIRE(sym2Z_.at("mg") == 12);
    REQUIRE(sym2Z_.at("al") == 13);
    REQUIRE(sym2Z_.at("si") == 14);
    REQUIRE(sym2Z_.at("p") == 15);
    REQUIRE(sym2Z_.at("s") == 16);
    REQUIRE(sym2Z_.at("cl") == 17);
    REQUIRE(sym2Z_.at("ar") == 18);
    REQUIRE(sym2Z_.at("k") == 19);
    REQUIRE(sym2Z_.at("ca") == 20);
    REQUIRE(sym2Z_.at("sc") == 21);
    REQUIRE(sym2Z_.at("ti") == 22);
    REQUIRE(sym2Z_.at("v") == 23);
    REQUIRE(sym2Z_.at("cr") == 24);
    REQUIRE(sym2Z_.at("mn") == 25);
    REQUIRE(sym2Z_.at("fe") == 26);
    REQUIRE(sym2Z_.at("co") == 27);
    REQUIRE(sym2Z_.at("ni") == 28);
    REQUIRE(sym2Z_.at("cu") == 29);
    REQUIRE(sym2Z_.at("zn") == 30);
    REQUIRE(sym2Z_.at("ga") == 31);
    REQUIRE(sym2Z_.at("ge") == 32);
    REQUIRE(sym2Z_.at("as") == 33);
    REQUIRE(sym2Z_.at("se") == 34);
    REQUIRE(sym2Z_.at("br") == 35);
    REQUIRE(sym2Z_.at("kr") == 36);
    REQUIRE(sym2Z_.at("rb") == 37);
    REQUIRE(sym2Z_.at("sr") == 38);
    REQUIRE(sym2Z_.at("y") == 39);
    REQUIRE(sym2Z_.at("zr") == 40);
    REQUIRE(sym2Z_.at("nb") == 41);
    REQUIRE(sym2Z_.at("mo") == 42);
    REQUIRE(sym2Z_.at("tc") == 43);
    REQUIRE(sym2Z_.at("ru") == 44);
    REQUIRE(sym2Z_.at("rh") == 45);
    REQUIRE(sym2Z_.at("pd") == 46);
    REQUIRE(sym2Z_.at("ag") == 47);
    REQUIRE(sym2Z_.at("cd") == 48);
    REQUIRE(sym2Z_.at("in") == 49);
    REQUIRE(sym2Z_.at("sn") == 50);
    REQUIRE(sym2Z_.at("sb") == 51);
    REQUIRE(sym2Z_.at("te") == 52);
    REQUIRE(sym2Z_.at("i") == 53);
    REQUIRE(sym2Z_.at("xe") == 54);
    REQUIRE(sym2Z_.at("cs") == 55);
    REQUIRE(sym2Z_.at("ba") == 56);
    REQUIRE(sym2Z_.at("la") == 57);
    REQUIRE(sym2Z_.at("ce") == 58);
    REQUIRE(sym2Z_.at("pr") == 59);
    REQUIRE(sym2Z_.at("nd") == 60);
    REQUIRE(sym2Z_.at("pm") == 61);
    REQUIRE(sym2Z_.at("sm") == 62);
    REQUIRE(sym2Z_.at("eu") == 63);
    REQUIRE(sym2Z_.at("gd") == 64);
    REQUIRE(sym2Z_.at("tb") == 65);
    REQUIRE(sym2Z_.at("dy") == 66);
    REQUIRE(sym2Z_.at("ho") == 67);
    REQUIRE(sym2Z_.at("er") == 68);
    REQUIRE(sym2Z_.at("tm") == 69);
    REQUIRE(sym2Z_.at("yb") == 70);
    REQUIRE(sym2Z_.at("lu") == 71);
    REQUIRE(sym2Z_.at("hf") == 72);
    REQUIRE(sym2Z_.at("ta") == 73);
    REQUIRE(sym2Z_.at("w") == 74);
    REQUIRE(sym2Z_.at("re") == 75);
    REQUIRE(sym2Z_.at("os") == 76);
    REQUIRE(sym2Z_.at("ir") == 77);
    REQUIRE(sym2Z_.at("pt") == 78);
    REQUIRE(sym2Z_.at("au") == 79);
    REQUIRE(sym2Z_.at("hg") == 80);
    REQUIRE(sym2Z_.at("tl") == 81);
    REQUIRE(sym2Z_.at("pb") == 82);
    REQUIRE(sym2Z_.at("bi") == 83);
    REQUIRE(sym2Z_.at("po") == 84);
    REQUIRE(sym2Z_.at("at") == 85);
    REQUIRE(sym2Z_.at("rn") == 86);
    REQUIRE(sym2Z_.at("fr") == 87);
    REQUIRE(sym2Z_.at("ra") == 88);
    REQUIRE(sym2Z_.at("ac") == 89);
    REQUIRE(sym2Z_.at("th") == 90);
    REQUIRE(sym2Z_.at("pa") == 91);
    REQUIRE(sym2Z_.at("u") == 92);
    REQUIRE(sym2Z_.at("np") == 93);
    REQUIRE(sym2Z_.at("pu") == 94);
    REQUIRE(sym2Z_.at("am") == 95);
    REQUIRE(sym2Z_.at("cm") == 96);
    REQUIRE(sym2Z_.at("bk") == 97);
    REQUIRE(sym2Z_.at("cf") == 98);
    REQUIRE(sym2Z_.at("es") == 99);
    REQUIRE(sym2Z_.at("fm") == 100);
    REQUIRE(sym2Z_.at("md") == 101);
    REQUIRE(sym2Z_.at("no") == 102);
    REQUIRE(sym2Z_.at("lr") == 103);
    REQUIRE(sym2Z_.at("rf") == 104);
    REQUIRE(sym2Z_.at("db") == 105);
    REQUIRE(sym2Z_.at("sg") == 106);
    REQUIRE(sym2Z_.at("bh") == 107);
    REQUIRE(sym2Z_.at("hs") == 108);
    REQUIRE(sym2Z_.at("mt") == 109);
    REQUIRE(sym2Z_.at("ds") == 110);
    REQUIRE(sym2Z_.at("rg") == 111);
    REQUIRE(sym2Z_.at("cn") == 112);
    REQUIRE(sym2Z_.at("nh") == 113);
    REQUIRE(sym2Z_.at("fl") == 114);
    REQUIRE(sym2Z_.at("mc") == 115);
    REQUIRE(sym2Z_.at("lv") == 116);
    REQUIRE(sym2Z_.at("ts") == 117);
    REQUIRE(sym2Z_.at("og") == 118);
    REQUIRE(sym2Z_.at("chg") == 999);
    REQUIRE(sym2Z_.at("dum") == 9999);
}

TEST_CASE("Atomic Data")
{
    SECTION("Atom # 0")
    {
        REQUIRE(atomic_data_.at(0).Z == 0);
        REQUIRE(atomic_data_.at(0).sym == "gho");
        REQUIRE(atomic_data_.at(0).name == "ghost");
        REQUIRE(atomic_data_.at(0).multiplicity == 0);
        REQUIRE(atomic_data_.at(0).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(0).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(0).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(0).isotopes.at(0).isonum == 0);
        REQUIRE(atomic_data_.at(0).isotopes.at(0).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(0).isotopes.at(0).abundance == Approx(0.0000000000000000));
    }
    SECTION("Atom # 1")
    {
        REQUIRE(atomic_data_.at(1).Z == 1);
        REQUIRE(atomic_data_.at(1).sym == "h");
        REQUIRE(atomic_data_.at(1).name == "hydrogen");
        REQUIRE(atomic_data_.at(1).multiplicity == 2);
        REQUIRE(atomic_data_.at(1).mass == Approx(1.0079750000000001));
        REQUIRE(atomic_data_.at(1).covradius == Approx(0.5858150988919267));
        REQUIRE(atomic_data_.at(1).vdwradius == Approx(2.2676713505493939));
        REQUIRE(atomic_data_.at(1).isotopes.at(1).isonum == 1);
        REQUIRE(atomic_data_.at(1).isotopes.at(1).mass == Approx(1.0078250322000000));
        REQUIRE(atomic_data_.at(1).isotopes.at(1).abundance == Approx(0.9998550000000000));
        REQUIRE(atomic_data_.at(1).isotopes.at(2).isonum == 2);
        REQUIRE(atomic_data_.at(1).isotopes.at(2).mass == Approx(2.0141017781000001));
        REQUIRE(atomic_data_.at(1).isotopes.at(2).abundance == Approx(0.0001450000000000));
    }
    SECTION("Atom # 2")
    {
        REQUIRE(atomic_data_.at(2).Z == 2);
        REQUIRE(atomic_data_.at(2).sym == "he");
        REQUIRE(atomic_data_.at(2).name == "helium");
        REQUIRE(atomic_data_.at(2).multiplicity == 1);
        REQUIRE(atomic_data_.at(2).mass == Approx(4.0026020000000004));
        REQUIRE(atomic_data_.at(2).covradius == Approx(0.5291233151281919));
        REQUIRE(atomic_data_.at(2).vdwradius == Approx(2.6456165756409593));
        REQUIRE(atomic_data_.at(2).isotopes.at(3).isonum == 3);
        REQUIRE(atomic_data_.at(2).isotopes.at(3).mass == Approx(3.0160293199999999));
        REQUIRE(atomic_data_.at(2).isotopes.at(3).abundance == Approx(0.0000020000000000));
        REQUIRE(atomic_data_.at(2).isotopes.at(4).isonum == 4);
        REQUIRE(atomic_data_.at(2).isotopes.at(4).mass == Approx(4.0026032541000003));
        REQUIRE(atomic_data_.at(2).isotopes.at(4).abundance == Approx(0.9999980000000001));
    }
    SECTION("Atom # 3")
    {
        REQUIRE(atomic_data_.at(3).Z == 3);
        REQUIRE(atomic_data_.at(3).sym == "li");
        REQUIRE(atomic_data_.at(3).name == "lithium");
        REQUIRE(atomic_data_.at(3).multiplicity == 2);
        REQUIRE(atomic_data_.at(3).mass == Approx(6.9675000000000002));
        REQUIRE(atomic_data_.at(3).covradius == Approx(2.4188494405860199));
        REQUIRE(atomic_data_.at(3).vdwradius == Approx(3.4393015483332472));
        REQUIRE(atomic_data_.at(3).isotopes.at(6).isonum == 6);
        REQUIRE(atomic_data_.at(3).isotopes.at(6).mass == Approx(6.0151228870000004));
        REQUIRE(atomic_data_.at(3).isotopes.at(6).abundance == Approx(0.0485000000000000));
        REQUIRE(atomic_data_.at(3).isotopes.at(7).isonum == 7);
        REQUIRE(atomic_data_.at(3).isotopes.at(7).mass == Approx(7.0160034400000004));
        REQUIRE(atomic_data_.at(3).isotopes.at(7).abundance == Approx(0.9515000000000000));
    }
    SECTION("Atom # 4")
    {
        REQUIRE(atomic_data_.at(4).Z == 4);
        REQUIRE(atomic_data_.at(4).sym == "be");
        REQUIRE(atomic_data_.at(4).name == "beryllium");
        REQUIRE(atomic_data_.at(4).multiplicity == 1);
        REQUIRE(atomic_data_.at(4).mass == Approx(9.0121830999999997));
        REQUIRE(atomic_data_.at(4).covradius == Approx(1.8141370804395149));
        REQUIRE(atomic_data_.at(4).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(4).isotopes.at(9).isonum == 9);
        REQUIRE(atomic_data_.at(4).isotopes.at(9).mass == Approx(9.0121830999999997));
        REQUIRE(atomic_data_.at(4).isotopes.at(9).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 5")
    {
        REQUIRE(atomic_data_.at(5).Z == 5);
        REQUIRE(atomic_data_.at(5).sym == "b");
        REQUIRE(atomic_data_.at(5).name == "boron");
        REQUIRE(atomic_data_.at(5).multiplicity == 2);
        REQUIRE(atomic_data_.at(5).mass == Approx(10.8134999999999994));
        REQUIRE(atomic_data_.at(5).covradius == Approx(1.6062672066391539));
        REQUIRE(atomic_data_.at(5).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(5).isotopes.at(10).isonum == 10);
        REQUIRE(atomic_data_.at(5).isotopes.at(10).mass == Approx(10.0129370000000009));
        REQUIRE(atomic_data_.at(5).isotopes.at(10).abundance == Approx(0.1965000000000000));
        REQUIRE(atomic_data_.at(5).isotopes.at(11).isonum == 11);
        REQUIRE(atomic_data_.at(5).isotopes.at(11).mass == Approx(11.0093049999999995));
        REQUIRE(atomic_data_.at(5).isotopes.at(11).abundance == Approx(0.8035000000000000));
    }
    SECTION("Atom # 6")
    {
        REQUIRE(atomic_data_.at(6).Z == 6);
        REQUIRE(atomic_data_.at(6).sym == "c");
        REQUIRE(atomic_data_.at(6).name == "carbon");
        REQUIRE(atomic_data_.at(6).multiplicity == 3);
        REQUIRE(atomic_data_.at(6).mass == Approx(12.0106000000000002));
        REQUIRE(atomic_data_.at(6).covradius == Approx(1.4361918553479494));
        REQUIRE(atomic_data_.at(6).vdwradius == Approx(3.2125344132783078));
        REQUIRE(atomic_data_.at(6).isotopes.at(12).isonum == 12);
        REQUIRE(atomic_data_.at(6).isotopes.at(12).mass == Approx(12.0000000000000000));
        REQUIRE(atomic_data_.at(6).isotopes.at(12).abundance == Approx(0.9893999999999999));
        REQUIRE(atomic_data_.at(6).isotopes.at(13).isonum == 13);
        REQUIRE(atomic_data_.at(6).isotopes.at(13).mass == Approx(13.0033548349999997));
        REQUIRE(atomic_data_.at(6).isotopes.at(13).abundance == Approx(0.0106000000000000));
    }
    SECTION("Atom # 7")
    {
        REQUIRE(atomic_data_.at(7).Z == 7);
        REQUIRE(atomic_data_.at(7).sym == "n");
        REQUIRE(atomic_data_.at(7).name == "nitrogen");
        REQUIRE(atomic_data_.at(7).multiplicity == 4);
        REQUIRE(atomic_data_.at(7).mass == Approx(14.0068549999999998));
        REQUIRE(atomic_data_.at(7).covradius == Approx(1.3417055490750580));
        REQUIRE(atomic_data_.at(7).vdwradius == Approx(2.9290754944596338));
        REQUIRE(atomic_data_.at(7).isotopes.at(14).isonum == 14);
        REQUIRE(atomic_data_.at(7).isotopes.at(14).mass == Approx(14.0030740040000001));
        REQUIRE(atomic_data_.at(7).isotopes.at(14).abundance == Approx(0.9962050000000000));
        REQUIRE(atomic_data_.at(7).isotopes.at(15).isonum == 15);
        REQUIRE(atomic_data_.at(7).isotopes.at(15).mass == Approx(15.0001088990000007));
        REQUIRE(atomic_data_.at(7).isotopes.at(15).abundance == Approx(0.0037950000000000));
    }
    SECTION("Atom # 8")
    {
        REQUIRE(atomic_data_.at(8).Z == 8);
        REQUIRE(atomic_data_.at(8).sym == "o");
        REQUIRE(atomic_data_.at(8).name == "oxygen");
        REQUIRE(atomic_data_.at(8).multiplicity == 3);
        REQUIRE(atomic_data_.at(8).mass == Approx(15.9993999999999996));
        REQUIRE(atomic_data_.at(8).covradius == Approx(1.2472192428021665));
        REQUIRE(atomic_data_.at(8).vdwradius == Approx(2.8723837106958987));
        REQUIRE(atomic_data_.at(8).isotopes.at(16).isonum == 16);
        REQUIRE(atomic_data_.at(8).isotopes.at(16).mass == Approx(15.9949146199999994));
        REQUIRE(atomic_data_.at(8).isotopes.at(16).abundance == Approx(0.9975700000000000));
        REQUIRE(atomic_data_.at(8).isotopes.at(17).isonum == 17);
        REQUIRE(atomic_data_.at(8).isotopes.at(17).mass == Approx(16.9991317570000007));
        REQUIRE(atomic_data_.at(8).isotopes.at(17).abundance == Approx(0.0003835000000000));
        REQUIRE(atomic_data_.at(8).isotopes.at(18).isonum == 18);
        REQUIRE(atomic_data_.at(8).isotopes.at(18).mass == Approx(17.9991596129999998));
        REQUIRE(atomic_data_.at(8).isotopes.at(18).abundance == Approx(0.0020450000000000));
    }
    SECTION("Atom # 9")
    {
        REQUIRE(atomic_data_.at(9).Z == 9);
        REQUIRE(atomic_data_.at(9).sym == "f");
        REQUIRE(atomic_data_.at(9).name == "fluorine");
        REQUIRE(atomic_data_.at(9).multiplicity == 2);
        REQUIRE(atomic_data_.at(9).mass == Approx(18.9984031629999990));
        REQUIRE(atomic_data_.at(9).covradius == Approx(1.0771438915109621));
        REQUIRE(atomic_data_.at(9).vdwradius == Approx(2.7778974044230074));
        REQUIRE(atomic_data_.at(9).isotopes.at(19).isonum == 19);
        REQUIRE(atomic_data_.at(9).isotopes.at(19).mass == Approx(18.9984031629999990));
        REQUIRE(atomic_data_.at(9).isotopes.at(19).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 10")
    {
        REQUIRE(atomic_data_.at(10).Z == 10);
        REQUIRE(atomic_data_.at(10).sym == "ne");
        REQUIRE(atomic_data_.at(10).name == "neon");
        REQUIRE(atomic_data_.at(10).multiplicity == 1);
        REQUIRE(atomic_data_.at(10).mass == Approx(20.1797000000000004));
        REQUIRE(atomic_data_.at(10).covradius == Approx(1.0960411527655403));
        REQUIRE(atomic_data_.at(10).vdwradius == Approx(2.9101782332050554));
        REQUIRE(atomic_data_.at(10).isotopes.at(20).isonum == 20);
        REQUIRE(atomic_data_.at(10).isotopes.at(20).mass == Approx(19.9924401799999991));
        REQUIRE(atomic_data_.at(10).isotopes.at(20).abundance == Approx(0.9048000000000000));
        REQUIRE(atomic_data_.at(10).isotopes.at(21).isonum == 21);
        REQUIRE(atomic_data_.at(10).isotopes.at(21).mass == Approx(20.9938466999999989));
        REQUIRE(atomic_data_.at(10).isotopes.at(21).abundance == Approx(0.0027000000000000));
        REQUIRE(atomic_data_.at(10).isotopes.at(22).isonum == 22);
        REQUIRE(atomic_data_.at(10).isotopes.at(22).mass == Approx(21.9913850999999987));
        REQUIRE(atomic_data_.at(10).isotopes.at(22).abundance == Approx(0.0925000000000000));
    }
    SECTION("Atom # 11")
    {
        REQUIRE(atomic_data_.at(11).Z == 11);
        REQUIRE(atomic_data_.at(11).sym == "na");
        REQUIRE(atomic_data_.at(11).name == "sodium");
        REQUIRE(atomic_data_.at(11).multiplicity == 2);
        REQUIRE(atomic_data_.at(11).mass == Approx(22.9897692800000009));
        REQUIRE(atomic_data_.at(11).covradius == Approx(3.1369453682599948));
        REQUIRE(atomic_data_.at(11).vdwradius == Approx(4.2896783047892697));
        REQUIRE(atomic_data_.at(11).isotopes.at(23).isonum == 23);
        REQUIRE(atomic_data_.at(11).isotopes.at(23).mass == Approx(22.9897692800000009));
        REQUIRE(atomic_data_.at(11).isotopes.at(23).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 12")
    {
        REQUIRE(atomic_data_.at(12).Z == 12);
        REQUIRE(atomic_data_.at(12).sym == "mg");
        REQUIRE(atomic_data_.at(12).name == "magnesium");
        REQUIRE(atomic_data_.at(12).multiplicity == 1);
        REQUIRE(atomic_data_.at(12).mass == Approx(24.3054999999999986));
        REQUIRE(atomic_data_.at(12).covradius == Approx(2.6645138368955377));
        REQUIRE(atomic_data_.at(12).vdwradius == Approx(3.2692261970420424));
        REQUIRE(atomic_data_.at(12).isotopes.at(24).isonum == 24);
        REQUIRE(atomic_data_.at(12).isotopes.at(24).mass == Approx(23.9850417000000000));
        REQUIRE(atomic_data_.at(12).isotopes.at(24).abundance == Approx(0.7896500000000000));
        REQUIRE(atomic_data_.at(12).isotopes.at(25).isonum == 25);
        REQUIRE(atomic_data_.at(12).isotopes.at(25).mass == Approx(24.9858370000000001));
        REQUIRE(atomic_data_.at(12).isotopes.at(25).abundance == Approx(0.1001100000000000));
        REQUIRE(atomic_data_.at(12).isotopes.at(26).isonum == 26);
        REQUIRE(atomic_data_.at(12).isotopes.at(26).mass == Approx(25.9825930000000014));
        REQUIRE(atomic_data_.at(12).isotopes.at(26).abundance == Approx(0.1102500000000000));
    }
    SECTION("Atom # 13")
    {
        REQUIRE(atomic_data_.at(13).Z == 13);
        REQUIRE(atomic_data_.at(13).sym == "al");
        REQUIRE(atomic_data_.at(13).name == "aluminum");
        REQUIRE(atomic_data_.at(13).multiplicity == 2);
        REQUIRE(atomic_data_.at(13).mass == Approx(26.9815384999999992));
        REQUIRE(atomic_data_.at(13).covradius == Approx(2.2865686118039719));
        REQUIRE(atomic_data_.at(13).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(13).isotopes.at(27).isonum == 27);
        REQUIRE(atomic_data_.at(13).isotopes.at(27).mass == Approx(26.9815384999999992));
        REQUIRE(atomic_data_.at(13).isotopes.at(27).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 14")
    {
        REQUIRE(atomic_data_.at(14).Z == 14);
        REQUIRE(atomic_data_.at(14).sym == "si");
        REQUIRE(atomic_data_.at(14).name == "silicon");
        REQUIRE(atomic_data_.at(14).multiplicity == 3);
        REQUIRE(atomic_data_.at(14).mass == Approx(28.0850000000000009));
        REQUIRE(atomic_data_.at(14).covradius == Approx(2.0975959992581892));
        REQUIRE(atomic_data_.at(14).vdwradius == Approx(3.9684248634614390));
        REQUIRE(atomic_data_.at(14).isotopes.at(28).isonum == 28);
        REQUIRE(atomic_data_.at(14).isotopes.at(28).mass == Approx(27.9769265350000005));
        REQUIRE(atomic_data_.at(14).isotopes.at(28).abundance == Approx(0.9225449999999999));
        REQUIRE(atomic_data_.at(14).isotopes.at(29).isonum == 29);
        REQUIRE(atomic_data_.at(14).isotopes.at(29).mass == Approx(28.9764946650000006));
        REQUIRE(atomic_data_.at(14).isotopes.at(29).abundance == Approx(0.0467200000000000));
        REQUIRE(atomic_data_.at(14).isotopes.at(30).isonum == 30);
        REQUIRE(atomic_data_.at(14).isotopes.at(30).mass == Approx(29.9737700099999991));
        REQUIRE(atomic_data_.at(14).isotopes.at(30).abundance == Approx(0.0307350000000000));
    }
    SECTION("Atom # 15")
    {
        REQUIRE(atomic_data_.at(15).Z == 15);
        REQUIRE(atomic_data_.at(15).sym == "p");
        REQUIRE(atomic_data_.at(15).name == "phosphorus");
        REQUIRE(atomic_data_.at(15).multiplicity == 4);
        REQUIRE(atomic_data_.at(15).mass == Approx(30.9737619980000005));
        REQUIRE(atomic_data_.at(15).covradius == Approx(2.0220069542398762));
        REQUIRE(atomic_data_.at(15).vdwradius == Approx(3.4015070258240905));
        REQUIRE(atomic_data_.at(15).isotopes.at(31).isonum == 31);
        REQUIRE(atomic_data_.at(15).isotopes.at(31).mass == Approx(30.9737619980000005));
        REQUIRE(atomic_data_.at(15).isotopes.at(31).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 16")
    {
        REQUIRE(atomic_data_.at(16).Z == 16);
        REQUIRE(atomic_data_.at(16).sym == "s");
        REQUIRE(atomic_data_.at(16).name == "sulfur");
        REQUIRE(atomic_data_.at(16).multiplicity == 3);
        REQUIRE(atomic_data_.at(16).mass == Approx(32.0675000000000026));
        REQUIRE(atomic_data_.at(16).covradius == Approx(1.9842124317307195));
        REQUIRE(atomic_data_.at(16).vdwradius == Approx(3.4015070258240905));
        REQUIRE(atomic_data_.at(16).isotopes.at(32).isonum == 32);
        REQUIRE(atomic_data_.at(16).isotopes.at(32).mass == Approx(31.9720711739999999));
        REQUIRE(atomic_data_.at(16).isotopes.at(32).abundance == Approx(0.9485000000000000));
        REQUIRE(atomic_data_.at(16).isotopes.at(33).isonum == 33);
        REQUIRE(atomic_data_.at(16).isotopes.at(33).mass == Approx(32.9714589100000026));
        REQUIRE(atomic_data_.at(16).isotopes.at(33).abundance == Approx(0.0076300000000000));
        REQUIRE(atomic_data_.at(16).isotopes.at(34).isonum == 34);
        REQUIRE(atomic_data_.at(16).isotopes.at(34).mass == Approx(33.9678669999999983));
        REQUIRE(atomic_data_.at(16).isotopes.at(34).abundance == Approx(0.0436500000000000));
        REQUIRE(atomic_data_.at(16).isotopes.at(36).isonum == 36);
        REQUIRE(atomic_data_.at(16).isotopes.at(36).mass == Approx(35.9670810000000003));
        REQUIRE(atomic_data_.at(16).isotopes.at(36).abundance == Approx(0.0001580000000000));
    }
    SECTION("Atom # 17")
    {
        REQUIRE(atomic_data_.at(17).Z == 17);
        REQUIRE(atomic_data_.at(17).sym == "cl");
        REQUIRE(atomic_data_.at(17).name == "chlorine");
        REQUIRE(atomic_data_.at(17).multiplicity == 2);
        REQUIRE(atomic_data_.at(17).mass == Approx(35.4515000000000029));
        REQUIRE(atomic_data_.at(17).covradius == Approx(1.9275206479669846));
        REQUIRE(atomic_data_.at(17).vdwradius == Approx(3.3070207195511991));
        REQUIRE(atomic_data_.at(17).isotopes.at(35).isonum == 35);
        REQUIRE(atomic_data_.at(17).isotopes.at(35).mass == Approx(34.9688526999999993));
        REQUIRE(atomic_data_.at(17).isotopes.at(35).abundance == Approx(0.7580000000000000));
        REQUIRE(atomic_data_.at(17).isotopes.at(37).isonum == 37);
        REQUIRE(atomic_data_.at(17).isotopes.at(37).mass == Approx(36.9659025999999997));
        REQUIRE(atomic_data_.at(17).isotopes.at(37).abundance == Approx(0.2420000000000000));
    }
    SECTION("Atom # 18")
    {
        REQUIRE(atomic_data_.at(18).Z == 18);
        REQUIRE(atomic_data_.at(18).sym == "ar");
        REQUIRE(atomic_data_.at(18).name == "argon");
        REQUIRE(atomic_data_.at(18).multiplicity == 1);
        REQUIRE(atomic_data_.at(18).mass == Approx(39.9480000000000004));
        REQUIRE(atomic_data_.at(18).covradius == Approx(2.0031096929852978));
        REQUIRE(atomic_data_.at(18).vdwradius == Approx(3.5526851158607169));
        REQUIRE(atomic_data_.at(18).isotopes.at(36).isonum == 36);
        REQUIRE(atomic_data_.at(18).isotopes.at(36).mass == Approx(35.9675451000000024));
        REQUIRE(atomic_data_.at(18).isotopes.at(36).abundance == Approx(0.0033360000000000));
        REQUIRE(atomic_data_.at(18).isotopes.at(38).isonum == 38);
        REQUIRE(atomic_data_.at(18).isotopes.at(38).mass == Approx(37.9627320000000026));
        REQUIRE(atomic_data_.at(18).isotopes.at(38).abundance == Approx(0.0006290000000000));
        REQUIRE(atomic_data_.at(18).isotopes.at(40).isonum == 40);
        REQUIRE(atomic_data_.at(18).isotopes.at(40).mass == Approx(39.9623831199999984));
        REQUIRE(atomic_data_.at(18).isotopes.at(40).abundance == Approx(0.9960350000000000));
    }
    SECTION("Atom # 19")
    {
        REQUIRE(atomic_data_.at(19).Z == 19);
        REQUIRE(atomic_data_.at(19).sym == "k");
        REQUIRE(atomic_data_.at(19).name == "potassium");
        REQUIRE(atomic_data_.at(19).multiplicity == 2);
        REQUIRE(atomic_data_.at(19).mass == Approx(39.0983000000000018));
        REQUIRE(atomic_data_.at(19).covradius == Approx(3.8361440346793909));
        REQUIRE(atomic_data_.at(19).vdwradius == Approx(5.1967468450090273));
        REQUIRE(atomic_data_.at(19).isotopes.at(39).isonum == 39);
        REQUIRE(atomic_data_.at(19).isotopes.at(39).mass == Approx(38.9637064899999999));
        REQUIRE(atomic_data_.at(19).isotopes.at(39).abundance == Approx(0.9325810000000000));
        REQUIRE(atomic_data_.at(19).isotopes.at(40).isonum == 40);
        REQUIRE(atomic_data_.at(19).isotopes.at(40).mass == Approx(39.9639981999999989));
        REQUIRE(atomic_data_.at(19).isotopes.at(40).abundance == Approx(0.0001170000000000));
        REQUIRE(atomic_data_.at(19).isotopes.at(41).isonum == 41);
        REQUIRE(atomic_data_.at(19).isotopes.at(41).mass == Approx(40.9618252599999977));
        REQUIRE(atomic_data_.at(19).isotopes.at(41).abundance == Approx(0.0673020000000000));
    }
    SECTION("Atom # 20")
    {
        REQUIRE(atomic_data_.at(20).Z == 20);
        REQUIRE(atomic_data_.at(20).sym == "ca");
        REQUIRE(atomic_data_.at(20).name == "calcium");
        REQUIRE(atomic_data_.at(20).multiplicity == 1);
        REQUIRE(atomic_data_.at(20).mass == Approx(40.0780000000000030));
        REQUIRE(atomic_data_.at(20).covradius == Approx(3.3259179808057775));
        REQUIRE(atomic_data_.at(20).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(20).isotopes.at(40).isonum == 40);
        REQUIRE(atomic_data_.at(20).isotopes.at(40).mass == Approx(39.9625909000000021));
        REQUIRE(atomic_data_.at(20).isotopes.at(40).abundance == Approx(0.9694100000000000));
        REQUIRE(atomic_data_.at(20).isotopes.at(42).isonum == 42);
        REQUIRE(atomic_data_.at(20).isotopes.at(42).mass == Approx(41.9586180000000013));
        REQUIRE(atomic_data_.at(20).isotopes.at(42).abundance == Approx(0.0064700000000000));
        REQUIRE(atomic_data_.at(20).isotopes.at(43).isonum == 43);
        REQUIRE(atomic_data_.at(20).isotopes.at(43).mass == Approx(42.9587659999999971));
        REQUIRE(atomic_data_.at(20).isotopes.at(43).abundance == Approx(0.0013500000000000));
        REQUIRE(atomic_data_.at(20).isotopes.at(44).isonum == 44);
        REQUIRE(atomic_data_.at(20).isotopes.at(44).mass == Approx(43.9554820000000035));
        REQUIRE(atomic_data_.at(20).isotopes.at(44).abundance == Approx(0.0208600000000000));
        REQUIRE(atomic_data_.at(20).isotopes.at(46).isonum == 46);
        REQUIRE(atomic_data_.at(20).isotopes.at(46).mass == Approx(45.9536900000000017));
        REQUIRE(atomic_data_.at(20).isotopes.at(46).abundance == Approx(0.0000400000000000));
        REQUIRE(atomic_data_.at(20).isotopes.at(48).isonum == 48);
        REQUIRE(atomic_data_.at(20).isotopes.at(48).mass == Approx(47.9525227999999970));
        REQUIRE(atomic_data_.at(20).isotopes.at(48).abundance == Approx(0.0018700000000000));
    }
    SECTION("Atom # 21")
    {
        REQUIRE(atomic_data_.at(21).Z == 21);
        REQUIRE(atomic_data_.at(21).sym == "sc");
        REQUIRE(atomic_data_.at(21).name == "scandium");
        REQUIRE(atomic_data_.at(21).multiplicity == 2);
        REQUIRE(atomic_data_.at(21).mass == Approx(44.9559080000000009));
        REQUIRE(atomic_data_.at(21).covradius == Approx(3.2125344132783078));
        REQUIRE(atomic_data_.at(21).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(21).isotopes.at(45).isonum == 45);
        REQUIRE(atomic_data_.at(21).isotopes.at(45).mass == Approx(44.9559080000000009));
        REQUIRE(atomic_data_.at(21).isotopes.at(45).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 22")
    {
        REQUIRE(atomic_data_.at(22).Z == 22);
        REQUIRE(atomic_data_.at(22).sym == "ti");
        REQUIRE(atomic_data_.at(22).name == "titanium");
        REQUIRE(atomic_data_.at(22).multiplicity == 3);
        REQUIRE(atomic_data_.at(22).mass == Approx(47.8669999999999973));
        REQUIRE(atomic_data_.at(22).covradius == Approx(3.0235618007325251));
        REQUIRE(atomic_data_.at(22).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(22).isotopes.at(46).isonum == 46);
        REQUIRE(atomic_data_.at(22).isotopes.at(46).mass == Approx(45.9526279999999971));
        REQUIRE(atomic_data_.at(22).isotopes.at(46).abundance == Approx(0.0825000000000000));
        REQUIRE(atomic_data_.at(22).isotopes.at(47).isonum == 47);
        REQUIRE(atomic_data_.at(22).isotopes.at(47).mass == Approx(46.9517590000000027));
        REQUIRE(atomic_data_.at(22).isotopes.at(47).abundance == Approx(0.0744000000000000));
        REQUIRE(atomic_data_.at(22).isotopes.at(48).isonum == 48);
        REQUIRE(atomic_data_.at(22).isotopes.at(48).mass == Approx(47.9479419999999976));
        REQUIRE(atomic_data_.at(22).isotopes.at(48).abundance == Approx(0.7372000000000000));
        REQUIRE(atomic_data_.at(22).isotopes.at(49).isonum == 49);
        REQUIRE(atomic_data_.at(22).isotopes.at(49).mass == Approx(48.9478659999999977));
        REQUIRE(atomic_data_.at(22).isotopes.at(49).abundance == Approx(0.0541000000000000));
        REQUIRE(atomic_data_.at(22).isotopes.at(50).isonum == 50);
        REQUIRE(atomic_data_.at(22).isotopes.at(50).mass == Approx(49.9447869999999980));
        REQUIRE(atomic_data_.at(22).isotopes.at(50).abundance == Approx(0.0518000000000000));
    }
    SECTION("Atom # 23")
    {
        REQUIRE(atomic_data_.at(23).Z == 23);
        REQUIRE(atomic_data_.at(23).sym == "v");
        REQUIRE(atomic_data_.at(23).name == "vanadium");
        REQUIRE(atomic_data_.at(23).multiplicity == 4);
        REQUIRE(atomic_data_.at(23).mass == Approx(50.9414999999999978));
        REQUIRE(atomic_data_.at(23).covradius == Approx(2.8912809719504771));
        REQUIRE(atomic_data_.at(23).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(23).isotopes.at(50).isonum == 50);
        REQUIRE(atomic_data_.at(23).isotopes.at(50).mass == Approx(49.9471559999999997));
        REQUIRE(atomic_data_.at(23).isotopes.at(50).abundance == Approx(0.0025000000000000));
        REQUIRE(atomic_data_.at(23).isotopes.at(51).isonum == 51);
        REQUIRE(atomic_data_.at(23).isotopes.at(51).mass == Approx(50.9439569999999975));
        REQUIRE(atomic_data_.at(23).isotopes.at(51).abundance == Approx(0.9975000000000001));
    }
    SECTION("Atom # 24")
    {
        REQUIRE(atomic_data_.at(24).Z == 24);
        REQUIRE(atomic_data_.at(24).sym == "cr");
        REQUIRE(atomic_data_.at(24).name == "chromium");
        REQUIRE(atomic_data_.at(24).multiplicity == 7);
        REQUIRE(atomic_data_.at(24).mass == Approx(51.9960999999999984));
        REQUIRE(atomic_data_.at(24).covradius == Approx(2.6267193143863810));
        REQUIRE(atomic_data_.at(24).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(24).isotopes.at(50).isonum == 50);
        REQUIRE(atomic_data_.at(24).isotopes.at(50).mass == Approx(49.9460419999999985));
        REQUIRE(atomic_data_.at(24).isotopes.at(50).abundance == Approx(0.0434500000000000));
        REQUIRE(atomic_data_.at(24).isotopes.at(52).isonum == 52);
        REQUIRE(atomic_data_.at(24).isotopes.at(52).mass == Approx(51.9405059999999992));
        REQUIRE(atomic_data_.at(24).isotopes.at(52).abundance == Approx(0.8378900000000000));
        REQUIRE(atomic_data_.at(24).isotopes.at(53).isonum == 53);
        REQUIRE(atomic_data_.at(24).isotopes.at(53).mass == Approx(52.9406480000000030));
        REQUIRE(atomic_data_.at(24).isotopes.at(53).abundance == Approx(0.0950100000000000));
        REQUIRE(atomic_data_.at(24).isotopes.at(54).isonum == 54);
        REQUIRE(atomic_data_.at(24).isotopes.at(54).mass == Approx(53.9388790000000000));
        REQUIRE(atomic_data_.at(24).isotopes.at(54).abundance == Approx(0.0236500000000000));
    }
    SECTION("Atom # 25")
    {
        REQUIRE(atomic_data_.at(25).Z == 25);
        REQUIRE(atomic_data_.at(25).sym == "mn");
        REQUIRE(atomic_data_.at(25).name == "manganese");
        REQUIRE(atomic_data_.at(25).multiplicity == 6);
        REQUIRE(atomic_data_.at(25).mass == Approx(54.9380439999999979));
        REQUIRE(atomic_data_.at(25).covradius == Approx(2.6267193143863810));
        REQUIRE(atomic_data_.at(25).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(25).isotopes.at(55).isonum == 55);
        REQUIRE(atomic_data_.at(25).isotopes.at(55).mass == Approx(54.9380439999999979));
        REQUIRE(atomic_data_.at(25).isotopes.at(55).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 26")
    {
        REQUIRE(atomic_data_.at(26).Z == 26);
        REQUIRE(atomic_data_.at(26).sym == "fe");
        REQUIRE(atomic_data_.at(26).name == "iron");
        REQUIRE(atomic_data_.at(26).multiplicity == 5);
        REQUIRE(atomic_data_.at(26).mass == Approx(55.8449999999999989));
        REQUIRE(atomic_data_.at(26).covradius == Approx(2.4944384856043329));
        REQUIRE(atomic_data_.at(26).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(26).isotopes.at(54).isonum == 54);
        REQUIRE(atomic_data_.at(26).isotopes.at(54).mass == Approx(53.9396089999999973));
        REQUIRE(atomic_data_.at(26).isotopes.at(54).abundance == Approx(0.0584500000000000));
        REQUIRE(atomic_data_.at(26).isotopes.at(56).isonum == 56);
        REQUIRE(atomic_data_.at(26).isotopes.at(56).mass == Approx(55.9349360000000004));
        REQUIRE(atomic_data_.at(26).isotopes.at(56).abundance == Approx(0.9175400000000000));
        REQUIRE(atomic_data_.at(26).isotopes.at(57).isonum == 57);
        REQUIRE(atomic_data_.at(26).isotopes.at(57).mass == Approx(56.9353929999999977));
        REQUIRE(atomic_data_.at(26).isotopes.at(57).abundance == Approx(0.0211900000000000));
        REQUIRE(atomic_data_.at(26).isotopes.at(58).isonum == 58);
        REQUIRE(atomic_data_.at(26).isotopes.at(58).mass == Approx(57.9332739999999973));
        REQUIRE(atomic_data_.at(26).isotopes.at(58).abundance == Approx(0.0028200000000000));
    }
    SECTION("Atom # 27")
    {
        REQUIRE(atomic_data_.at(27).Z == 27);
        REQUIRE(atomic_data_.at(27).sym == "co");
        REQUIRE(atomic_data_.at(27).name == "cobalt");
        REQUIRE(atomic_data_.at(27).multiplicity == 4);
        REQUIRE(atomic_data_.at(27).mass == Approx(58.9331940000000003));
        REQUIRE(atomic_data_.at(27).covradius == Approx(2.3810549180768632));
        REQUIRE(atomic_data_.at(27).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(27).isotopes.at(59).isonum == 59);
        REQUIRE(atomic_data_.at(27).isotopes.at(59).mass == Approx(58.9331940000000003));
        REQUIRE(atomic_data_.at(27).isotopes.at(59).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 28")
    {
        REQUIRE(atomic_data_.at(28).Z == 28);
        REQUIRE(atomic_data_.at(28).sym == "ni");
        REQUIRE(atomic_data_.at(28).name == "nickel");
        REQUIRE(atomic_data_.at(28).multiplicity == 3);
        REQUIRE(atomic_data_.at(28).mass == Approx(58.6933999999999969));
        REQUIRE(atomic_data_.at(28).covradius == Approx(2.3432603955677069));
        REQUIRE(atomic_data_.at(28).vdwradius == Approx(3.0802535844962597));
        REQUIRE(atomic_data_.at(28).isotopes.at(58).isonum == 58);
        REQUIRE(atomic_data_.at(28).isotopes.at(58).mass == Approx(57.9353419999999986));
        REQUIRE(atomic_data_.at(28).isotopes.at(58).abundance == Approx(0.6807690000000000));
        REQUIRE(atomic_data_.at(28).isotopes.at(60).isonum == 60);
        REQUIRE(atomic_data_.at(28).isotopes.at(60).mass == Approx(59.9307859999999977));
        REQUIRE(atomic_data_.at(28).isotopes.at(60).abundance == Approx(0.2622310000000000));
        REQUIRE(atomic_data_.at(28).isotopes.at(61).isonum == 61);
        REQUIRE(atomic_data_.at(28).isotopes.at(61).mass == Approx(60.9310559999999981));
        REQUIRE(atomic_data_.at(28).isotopes.at(61).abundance == Approx(0.0113990000000000));
        REQUIRE(atomic_data_.at(28).isotopes.at(62).isonum == 62);
        REQUIRE(atomic_data_.at(28).isotopes.at(62).mass == Approx(61.9283450000000002));
        REQUIRE(atomic_data_.at(28).isotopes.at(62).abundance == Approx(0.0363450000000000));
        REQUIRE(atomic_data_.at(28).isotopes.at(64).isonum == 64);
        REQUIRE(atomic_data_.at(28).isotopes.at(64).mass == Approx(63.9279670000000024));
        REQUIRE(atomic_data_.at(28).isotopes.at(64).abundance == Approx(0.0092560000000000));
    }
    SECTION("Atom # 29")
    {
        REQUIRE(atomic_data_.at(29).Z == 29);
        REQUIRE(atomic_data_.at(29).sym == "cu");
        REQUIRE(atomic_data_.at(29).name == "copper");
        REQUIRE(atomic_data_.at(29).multiplicity == 2);
        REQUIRE(atomic_data_.at(29).mass == Approx(63.5459999999999994));
        REQUIRE(atomic_data_.at(29).covradius == Approx(2.4944384856043329));
        REQUIRE(atomic_data_.at(29).vdwradius == Approx(2.6456165756409593));
        REQUIRE(atomic_data_.at(29).isotopes.at(63).isonum == 63);
        REQUIRE(atomic_data_.at(29).isotopes.at(63).mass == Approx(62.9295979999999986));
        REQUIRE(atomic_data_.at(29).isotopes.at(63).abundance == Approx(0.6915000000000000));
        REQUIRE(atomic_data_.at(29).isotopes.at(65).isonum == 65);
        REQUIRE(atomic_data_.at(29).isotopes.at(65).mass == Approx(64.9277900000000017));
        REQUIRE(atomic_data_.at(29).isotopes.at(65).abundance == Approx(0.3085000000000000));
    }
    SECTION("Atom # 30")
    {
        REQUIRE(atomic_data_.at(30).Z == 30);
        REQUIRE(atomic_data_.at(30).sym == "zn");
        REQUIRE(atomic_data_.at(30).name == "zinc");
        REQUIRE(atomic_data_.at(30).multiplicity == 1);
        REQUIRE(atomic_data_.at(30).mass == Approx(65.3799999999999955));
        REQUIRE(atomic_data_.at(30).covradius == Approx(2.3054658730585502));
        REQUIRE(atomic_data_.at(30).vdwradius == Approx(2.6267193143863810));
        REQUIRE(atomic_data_.at(30).isotopes.at(64).isonum == 64);
        REQUIRE(atomic_data_.at(30).isotopes.at(64).mass == Approx(63.9291419999999988));
        REQUIRE(atomic_data_.at(30).isotopes.at(64).abundance == Approx(0.4917000000000000));
        REQUIRE(atomic_data_.at(30).isotopes.at(66).isonum == 66);
        REQUIRE(atomic_data_.at(30).isotopes.at(66).mass == Approx(65.9260340000000014));
        REQUIRE(atomic_data_.at(30).isotopes.at(66).abundance == Approx(0.2773000000000000));
        REQUIRE(atomic_data_.at(30).isotopes.at(67).isonum == 67);
        REQUIRE(atomic_data_.at(30).isotopes.at(67).mass == Approx(66.9271279999999962));
        REQUIRE(atomic_data_.at(30).isotopes.at(67).abundance == Approx(0.0404000000000000));
        REQUIRE(atomic_data_.at(30).isotopes.at(68).isonum == 68);
        REQUIRE(atomic_data_.at(30).isotopes.at(68).mass == Approx(67.9248450000000048));
        REQUIRE(atomic_data_.at(30).isotopes.at(68).abundance == Approx(0.1845000000000000));
        REQUIRE(atomic_data_.at(30).isotopes.at(70).isonum == 70);
        REQUIRE(atomic_data_.at(30).isotopes.at(70).mass == Approx(69.9253199999999993));
        REQUIRE(atomic_data_.at(30).isotopes.at(70).abundance == Approx(0.0061000000000000));
    }
    SECTION("Atom # 31")
    {
        REQUIRE(atomic_data_.at(31).Z == 31);
        REQUIRE(atomic_data_.at(31).sym == "ga");
        REQUIRE(atomic_data_.at(31).name == "gallium");
        REQUIRE(atomic_data_.at(31).multiplicity == 2);
        REQUIRE(atomic_data_.at(31).mass == Approx(69.7229999999999990));
        REQUIRE(atomic_data_.at(31).covradius == Approx(2.3054658730585502));
        REQUIRE(atomic_data_.at(31).vdwradius == Approx(3.5337878546061385));
        REQUIRE(atomic_data_.at(31).isotopes.at(69).isonum == 69);
        REQUIRE(atomic_data_.at(31).isotopes.at(69).mass == Approx(68.9255739999999975));
        REQUIRE(atomic_data_.at(31).isotopes.at(69).abundance == Approx(0.6010799999999999));
        REQUIRE(atomic_data_.at(31).isotopes.at(71).isonum == 71);
        REQUIRE(atomic_data_.at(31).isotopes.at(71).mass == Approx(70.9247029999999938));
        REQUIRE(atomic_data_.at(31).isotopes.at(71).abundance == Approx(0.3989200000000000));
    }
    SECTION("Atom # 32")
    {
        REQUIRE(atomic_data_.at(32).Z == 32);
        REQUIRE(atomic_data_.at(32).sym == "ge");
        REQUIRE(atomic_data_.at(32).name == "germanium");
        REQUIRE(atomic_data_.at(32).multiplicity == 3);
        REQUIRE(atomic_data_.at(32).mass == Approx(72.6299999999999955));
        REQUIRE(atomic_data_.at(32).covradius == Approx(2.2676713505493939));
        REQUIRE(atomic_data_.at(32).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(32).isotopes.at(70).isonum == 70);
        REQUIRE(atomic_data_.at(32).isotopes.at(70).mass == Approx(69.9242490000000032));
        REQUIRE(atomic_data_.at(32).isotopes.at(70).abundance == Approx(0.2052000000000000));
        REQUIRE(atomic_data_.at(32).isotopes.at(72).isonum == 72);
        REQUIRE(atomic_data_.at(32).isotopes.at(72).mass == Approx(71.9220758000000018));
        REQUIRE(atomic_data_.at(32).isotopes.at(72).abundance == Approx(0.2745000000000000));
        REQUIRE(atomic_data_.at(32).isotopes.at(73).isonum == 73);
        REQUIRE(atomic_data_.at(32).isotopes.at(73).mass == Approx(72.9234589999999940));
        REQUIRE(atomic_data_.at(32).isotopes.at(73).abundance == Approx(0.0776000000000000));
        REQUIRE(atomic_data_.at(32).isotopes.at(74).isonum == 74);
        REQUIRE(atomic_data_.at(32).isotopes.at(74).mass == Approx(73.9211777600000062));
        REQUIRE(atomic_data_.at(32).isotopes.at(74).abundance == Approx(0.3652000000000000));
        REQUIRE(atomic_data_.at(32).isotopes.at(76).isonum == 76);
        REQUIRE(atomic_data_.at(32).isotopes.at(76).mass == Approx(75.9214027000000016));
        REQUIRE(atomic_data_.at(32).isotopes.at(76).abundance == Approx(0.0775000000000000));
    }
    SECTION("Atom # 33")
    {
        REQUIRE(atomic_data_.at(33).Z == 33);
        REQUIRE(atomic_data_.at(33).sym == "as");
        REQUIRE(atomic_data_.at(33).name == "arsenic");
        REQUIRE(atomic_data_.at(33).multiplicity == 4);
        REQUIRE(atomic_data_.at(33).mass == Approx(74.9215949999999964));
        REQUIRE(atomic_data_.at(33).covradius == Approx(2.2487740892948156));
        REQUIRE(atomic_data_.at(33).vdwradius == Approx(3.4959933320969818));
        REQUIRE(atomic_data_.at(33).isotopes.at(75).isonum == 75);
        REQUIRE(atomic_data_.at(33).isotopes.at(75).mass == Approx(74.9215949999999964));
        REQUIRE(atomic_data_.at(33).isotopes.at(75).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 34")
    {
        REQUIRE(atomic_data_.at(34).Z == 34);
        REQUIRE(atomic_data_.at(34).sym == "se");
        REQUIRE(atomic_data_.at(34).name == "selenium");
        REQUIRE(atomic_data_.at(34).multiplicity == 3);
        REQUIRE(atomic_data_.at(34).mass == Approx(78.9710000000000036));
        REQUIRE(atomic_data_.at(34).covradius == Approx(2.2676713505493939));
        REQUIRE(atomic_data_.at(34).vdwradius == Approx(3.5904796383698736));
        REQUIRE(atomic_data_.at(34).isotopes.at(74).isonum == 74);
        REQUIRE(atomic_data_.at(34).isotopes.at(74).mass == Approx(73.9224758999999949));
        REQUIRE(atomic_data_.at(34).isotopes.at(74).abundance == Approx(0.0086000000000000));
        REQUIRE(atomic_data_.at(34).isotopes.at(76).isonum == 76);
        REQUIRE(atomic_data_.at(34).isotopes.at(76).mass == Approx(75.9192137000000002));
        REQUIRE(atomic_data_.at(34).isotopes.at(76).abundance == Approx(0.0923000000000000));
        REQUIRE(atomic_data_.at(34).isotopes.at(77).isonum == 77);
        REQUIRE(atomic_data_.at(34).isotopes.at(77).mass == Approx(76.9199141999999938));
        REQUIRE(atomic_data_.at(34).isotopes.at(77).abundance == Approx(0.0760000000000000));
        REQUIRE(atomic_data_.at(34).isotopes.at(78).isonum == 78);
        REQUIRE(atomic_data_.at(34).isotopes.at(78).mass == Approx(77.9173090000000030));
        REQUIRE(atomic_data_.at(34).isotopes.at(78).abundance == Approx(0.2369000000000000));
        REQUIRE(atomic_data_.at(34).isotopes.at(80).isonum == 80);
        REQUIRE(atomic_data_.at(34).isotopes.at(80).mass == Approx(79.9165220000000005));
        REQUIRE(atomic_data_.at(34).isotopes.at(80).abundance == Approx(0.4980000000000000));
        REQUIRE(atomic_data_.at(34).isotopes.at(82).isonum == 82);
        REQUIRE(atomic_data_.at(34).isotopes.at(82).mass == Approx(81.9167000000000058));
        REQUIRE(atomic_data_.at(34).isotopes.at(82).abundance == Approx(0.0882000000000000));
    }
    SECTION("Atom # 35")
    {
        REQUIRE(atomic_data_.at(35).Z == 35);
        REQUIRE(atomic_data_.at(35).sym == "br");
        REQUIRE(atomic_data_.at(35).name == "bromine");
        REQUIRE(atomic_data_.at(35).multiplicity == 2);
        REQUIRE(atomic_data_.at(35).mass == Approx(79.9039999999999964));
        REQUIRE(atomic_data_.at(35).covradius == Approx(2.2676713505493939));
        REQUIRE(atomic_data_.at(35).vdwradius == Approx(3.4959933320969818));
        REQUIRE(atomic_data_.at(35).isotopes.at(79).isonum == 79);
        REQUIRE(atomic_data_.at(35).isotopes.at(79).mass == Approx(78.9183380000000056));
        REQUIRE(atomic_data_.at(35).isotopes.at(79).abundance == Approx(0.5065000000000000));
        REQUIRE(atomic_data_.at(35).isotopes.at(81).isonum == 81);
        REQUIRE(atomic_data_.at(35).isotopes.at(81).mass == Approx(80.9162900000000036));
        REQUIRE(atomic_data_.at(35).isotopes.at(81).abundance == Approx(0.4935000000000000));
    }
    SECTION("Atom # 36")
    {
        REQUIRE(atomic_data_.at(36).Z == 36);
        REQUIRE(atomic_data_.at(36).sym == "kr");
        REQUIRE(atomic_data_.at(36).name == "krypton");
        REQUIRE(atomic_data_.at(36).multiplicity == 1);
        REQUIRE(atomic_data_.at(36).mass == Approx(83.7980000000000018));
        REQUIRE(atomic_data_.at(36).covradius == Approx(2.1920823055310805));
        REQUIRE(atomic_data_.at(36).vdwradius == Approx(3.8172467734248130));
        REQUIRE(atomic_data_.at(36).isotopes.at(78).isonum == 78);
        REQUIRE(atomic_data_.at(36).isotopes.at(78).mass == Approx(77.9203650000000039));
        REQUIRE(atomic_data_.at(36).isotopes.at(78).abundance == Approx(0.0035500000000000));
        REQUIRE(atomic_data_.at(36).isotopes.at(80).isonum == 80);
        REQUIRE(atomic_data_.at(36).isotopes.at(80).mass == Approx(79.9163779999999946));
        REQUIRE(atomic_data_.at(36).isotopes.at(80).abundance == Approx(0.0228600000000000));
        REQUIRE(atomic_data_.at(36).isotopes.at(82).isonum == 82);
        REQUIRE(atomic_data_.at(36).isotopes.at(82).mass == Approx(81.9134829999999994));
        REQUIRE(atomic_data_.at(36).isotopes.at(82).abundance == Approx(0.1159300000000000));
        REQUIRE(atomic_data_.at(36).isotopes.at(83).isonum == 83);
        REQUIRE(atomic_data_.at(36).isotopes.at(83).mass == Approx(82.9141269999999935));
        REQUIRE(atomic_data_.at(36).isotopes.at(83).abundance == Approx(0.1150000000000000));
        REQUIRE(atomic_data_.at(36).isotopes.at(84).isonum == 84);
        REQUIRE(atomic_data_.at(36).isotopes.at(84).mass == Approx(83.9114977299999936));
        REQUIRE(atomic_data_.at(36).isotopes.at(84).abundance == Approx(0.5698700000000000));
        REQUIRE(atomic_data_.at(36).isotopes.at(86).isonum == 86);
        REQUIRE(atomic_data_.at(36).isotopes.at(86).mass == Approx(85.9106106299999936));
        REQUIRE(atomic_data_.at(36).isotopes.at(86).abundance == Approx(0.1727900000000000));
    }
    SECTION("Atom # 37")
    {
        REQUIRE(atomic_data_.at(37).Z == 37);
        REQUIRE(atomic_data_.at(37).sym == "rb");
        REQUIRE(atomic_data_.at(37).name == "rubidium");
        REQUIRE(atomic_data_.at(37).multiplicity == 2);
        REQUIRE(atomic_data_.at(37).mass == Approx(85.4677999999999969));
        REQUIRE(atomic_data_.at(37).covradius == Approx(4.1573974760072216));
        REQUIRE(atomic_data_.at(37).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(37).isotopes.at(85).isonum == 85);
        REQUIRE(atomic_data_.at(37).isotopes.at(85).mass == Approx(84.9117897400000032));
        REQUIRE(atomic_data_.at(37).isotopes.at(85).abundance == Approx(0.7217000000000000));
        REQUIRE(atomic_data_.at(37).isotopes.at(87).isonum == 87);
        REQUIRE(atomic_data_.at(37).isotopes.at(87).mass == Approx(86.9091805300000004));
        REQUIRE(atomic_data_.at(37).isotopes.at(87).abundance == Approx(0.2783000000000000));
    }
    SECTION("Atom # 38")
    {
        REQUIRE(atomic_data_.at(38).Z == 38);
        REQUIRE(atomic_data_.at(38).sym == "sr");
        REQUIRE(atomic_data_.at(38).name == "strontium");
        REQUIRE(atomic_data_.at(38).multiplicity == 1);
        REQUIRE(atomic_data_.at(38).mass == Approx(87.6200000000000045));
        REQUIRE(atomic_data_.at(38).covradius == Approx(3.6849659446427649));
        REQUIRE(atomic_data_.at(38).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(38).isotopes.at(84).isonum == 84);
        REQUIRE(atomic_data_.at(38).isotopes.at(84).mass == Approx(83.9134190000000046));
        REQUIRE(atomic_data_.at(38).isotopes.at(84).abundance == Approx(0.0056000000000000));
        REQUIRE(atomic_data_.at(38).isotopes.at(86).isonum == 86);
        REQUIRE(atomic_data_.at(38).isotopes.at(86).mass == Approx(85.9092610000000008));
        REQUIRE(atomic_data_.at(38).isotopes.at(86).abundance == Approx(0.0986000000000000));
        REQUIRE(atomic_data_.at(38).isotopes.at(87).isonum == 87);
        REQUIRE(atomic_data_.at(38).isotopes.at(87).mass == Approx(86.9088780000000014));
        REQUIRE(atomic_data_.at(38).isotopes.at(87).abundance == Approx(0.0700000000000000));
        REQUIRE(atomic_data_.at(38).isotopes.at(88).isonum == 88);
        REQUIRE(atomic_data_.at(38).isotopes.at(88).mass == Approx(87.9056130000000024));
        REQUIRE(atomic_data_.at(38).isotopes.at(88).abundance == Approx(0.8258000000000000));
    }
    SECTION("Atom # 39")
    {
        REQUIRE(atomic_data_.at(39).Z == 39);
        REQUIRE(atomic_data_.at(39).sym == "y");
        REQUIRE(atomic_data_.at(39).name == "yttrium");
        REQUIRE(atomic_data_.at(39).multiplicity == 2);
        REQUIRE(atomic_data_.at(39).mass == Approx(88.9058399999999978));
        REQUIRE(atomic_data_.at(39).covradius == Approx(3.5904796383698736));
        REQUIRE(atomic_data_.at(39).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(39).isotopes.at(89).isonum == 89);
        REQUIRE(atomic_data_.at(39).isotopes.at(89).mass == Approx(88.9058399999999978));
        REQUIRE(atomic_data_.at(39).isotopes.at(89).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 40")
    {
        REQUIRE(atomic_data_.at(40).Z == 40);
        REQUIRE(atomic_data_.at(40).sym == "zr");
        REQUIRE(atomic_data_.at(40).name == "zirconium");
        REQUIRE(atomic_data_.at(40).multiplicity == 3);
        REQUIRE(atomic_data_.at(40).mass == Approx(91.2240000000000038));
        REQUIRE(atomic_data_.at(40).covradius == Approx(3.3070207195511991));
        REQUIRE(atomic_data_.at(40).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(40).isotopes.at(90).isonum == 90);
        REQUIRE(atomic_data_.at(40).isotopes.at(90).mass == Approx(89.9047000000000054));
        REQUIRE(atomic_data_.at(40).isotopes.at(90).abundance == Approx(0.5145000000000000));
        REQUIRE(atomic_data_.at(40).isotopes.at(91).isonum == 91);
        REQUIRE(atomic_data_.at(40).isotopes.at(91).mass == Approx(90.9056400000000053));
        REQUIRE(atomic_data_.at(40).isotopes.at(91).abundance == Approx(0.1122000000000000));
        REQUIRE(atomic_data_.at(40).isotopes.at(92).isonum == 92);
        REQUIRE(atomic_data_.at(40).isotopes.at(92).mass == Approx(91.9050299999999964));
        REQUIRE(atomic_data_.at(40).isotopes.at(92).abundance == Approx(0.1715000000000000));
        REQUIRE(atomic_data_.at(40).isotopes.at(94).isonum == 94);
        REQUIRE(atomic_data_.at(40).isotopes.at(94).mass == Approx(93.9063100000000048));
        REQUIRE(atomic_data_.at(40).isotopes.at(94).abundance == Approx(0.1738000000000000));
        REQUIRE(atomic_data_.at(40).isotopes.at(96).isonum == 96);
        REQUIRE(atomic_data_.at(40).isotopes.at(96).mass == Approx(95.9082700000000017));
        REQUIRE(atomic_data_.at(40).isotopes.at(96).abundance == Approx(0.0280000000000000));
    }
    SECTION("Atom # 41")
    {
        REQUIRE(atomic_data_.at(41).Z == 41);
        REQUIRE(atomic_data_.at(41).sym == "nb");
        REQUIRE(atomic_data_.at(41).name == "niobium");
        REQUIRE(atomic_data_.at(41).multiplicity == 6);
        REQUIRE(atomic_data_.at(41).mass == Approx(92.9063699999999955));
        REQUIRE(atomic_data_.at(41).covradius == Approx(3.0991508457508381));
        REQUIRE(atomic_data_.at(41).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(41).isotopes.at(93).isonum == 93);
        REQUIRE(atomic_data_.at(41).isotopes.at(93).mass == Approx(92.9063699999999955));
        REQUIRE(atomic_data_.at(41).isotopes.at(93).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 42")
    {
        REQUIRE(atomic_data_.at(42).Z == 42);
        REQUIRE(atomic_data_.at(42).sym == "mo");
        REQUIRE(atomic_data_.at(42).name == "molybdenum");
        REQUIRE(atomic_data_.at(42).multiplicity == 7);
        REQUIRE(atomic_data_.at(42).mass == Approx(95.9500000000000028));
        REQUIRE(atomic_data_.at(42).covradius == Approx(2.9101782332050554));
        REQUIRE(atomic_data_.at(42).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(42).isotopes.at(92).isonum == 92);
        REQUIRE(atomic_data_.at(42).isotopes.at(92).mass == Approx(91.9068079999999981));
        REQUIRE(atomic_data_.at(42).isotopes.at(92).abundance == Approx(0.1464900000000000));
        REQUIRE(atomic_data_.at(42).isotopes.at(94).isonum == 94);
        REQUIRE(atomic_data_.at(42).isotopes.at(94).mass == Approx(93.9050849999999997));
        REQUIRE(atomic_data_.at(42).isotopes.at(94).abundance == Approx(0.0918700000000000));
        REQUIRE(atomic_data_.at(42).isotopes.at(95).isonum == 95);
        REQUIRE(atomic_data_.at(42).isotopes.at(95).mass == Approx(94.9058390000000003));
        REQUIRE(atomic_data_.at(42).isotopes.at(95).abundance == Approx(0.1587300000000000));
        REQUIRE(atomic_data_.at(42).isotopes.at(96).isonum == 96);
        REQUIRE(atomic_data_.at(42).isotopes.at(96).mass == Approx(95.9046759999999949));
        REQUIRE(atomic_data_.at(42).isotopes.at(96).abundance == Approx(0.1667300000000000));
        REQUIRE(atomic_data_.at(42).isotopes.at(97).isonum == 97);
        REQUIRE(atomic_data_.at(42).isotopes.at(97).mass == Approx(96.9060180000000031));
        REQUIRE(atomic_data_.at(42).isotopes.at(97).abundance == Approx(0.0958200000000000));
        REQUIRE(atomic_data_.at(42).isotopes.at(98).isonum == 98);
        REQUIRE(atomic_data_.at(42).isotopes.at(98).mass == Approx(97.9054050000000018));
        REQUIRE(atomic_data_.at(42).isotopes.at(98).abundance == Approx(0.2429200000000000));
        REQUIRE(atomic_data_.at(42).isotopes.at(100).isonum == 100);
        REQUIRE(atomic_data_.at(42).isotopes.at(100).mass == Approx(99.9074719999999985));
        REQUIRE(atomic_data_.at(42).isotopes.at(100).abundance == Approx(0.0974400000000000));
    }
    SECTION("Atom # 43")
    {
        REQUIRE(atomic_data_.at(43).Z == 43);
        REQUIRE(atomic_data_.at(43).sym == "tc");
        REQUIRE(atomic_data_.at(43).name == "technetium");
        REQUIRE(atomic_data_.at(43).multiplicity == 6);
        REQUIRE(atomic_data_.at(43).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(43).covradius == Approx(2.7778974044230074));
        REQUIRE(atomic_data_.at(43).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(43).isotopes.at(98).isonum == 98);
        REQUIRE(atomic_data_.at(43).isotopes.at(98).mass == Approx(97.9072100000000063));
        REQUIRE(atomic_data_.at(43).isotopes.at(98).abundance == Approx(0.0000000000000000));
    }
    SECTION("Atom # 44")
    {
        REQUIRE(atomic_data_.at(44).Z == 44);
        REQUIRE(atomic_data_.at(44).sym == "ru");
        REQUIRE(atomic_data_.at(44).name == "ruthenium");
        REQUIRE(atomic_data_.at(44).multiplicity == 5);
        REQUIRE(atomic_data_.at(44).mass == Approx(101.0699999999999932));
        REQUIRE(atomic_data_.at(44).covradius == Approx(2.7590001431684290));
        REQUIRE(atomic_data_.at(44).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(44).isotopes.at(96).isonum == 96);
        REQUIRE(atomic_data_.at(44).isotopes.at(96).mass == Approx(95.9075899999999990));
        REQUIRE(atomic_data_.at(44).isotopes.at(96).abundance == Approx(0.0554000000000000));
        REQUIRE(atomic_data_.at(44).isotopes.at(98).isonum == 98);
        REQUIRE(atomic_data_.at(44).isotopes.at(98).mass == Approx(97.9052899999999937));
        REQUIRE(atomic_data_.at(44).isotopes.at(98).abundance == Approx(0.0187000000000000));
        REQUIRE(atomic_data_.at(44).isotopes.at(99).isonum == 99);
        REQUIRE(atomic_data_.at(44).isotopes.at(99).mass == Approx(98.9059340000000020));
        REQUIRE(atomic_data_.at(44).isotopes.at(99).abundance == Approx(0.1276000000000000));
        REQUIRE(atomic_data_.at(44).isotopes.at(100).isonum == 100);
        REQUIRE(atomic_data_.at(44).isotopes.at(100).mass == Approx(99.9042139999999961));
        REQUIRE(atomic_data_.at(44).isotopes.at(100).abundance == Approx(0.1260000000000000));
        REQUIRE(atomic_data_.at(44).isotopes.at(101).isonum == 101);
        REQUIRE(atomic_data_.at(44).isotopes.at(101).mass == Approx(100.9055769999999939));
        REQUIRE(atomic_data_.at(44).isotopes.at(101).abundance == Approx(0.1706000000000000));
        REQUIRE(atomic_data_.at(44).isotopes.at(102).isonum == 102);
        REQUIRE(atomic_data_.at(44).isotopes.at(102).mass == Approx(101.9043439999999947));
        REQUIRE(atomic_data_.at(44).isotopes.at(102).abundance == Approx(0.3155000000000000));
        REQUIRE(atomic_data_.at(44).isotopes.at(104).isonum == 104);
        REQUIRE(atomic_data_.at(44).isotopes.at(104).mass == Approx(103.9054299999999955));
        REQUIRE(atomic_data_.at(44).isotopes.at(104).abundance == Approx(0.1862000000000000));
    }
    SECTION("Atom # 45")
    {
        REQUIRE(atomic_data_.at(45).Z == 45);
        REQUIRE(atomic_data_.at(45).sym == "rh");
        REQUIRE(atomic_data_.at(45).name == "rhodium");
        REQUIRE(atomic_data_.at(45).multiplicity == 4);
        REQUIRE(atomic_data_.at(45).mass == Approx(102.9055000000000035));
        REQUIRE(atomic_data_.at(45).covradius == Approx(2.6834110981501160));
        REQUIRE(atomic_data_.at(45).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(45).isotopes.at(103).isonum == 103);
        REQUIRE(atomic_data_.at(45).isotopes.at(103).mass == Approx(102.9055000000000035));
        REQUIRE(atomic_data_.at(45).isotopes.at(103).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 46")
    {
        REQUIRE(atomic_data_.at(46).Z == 46);
        REQUIRE(atomic_data_.at(46).sym == "pd");
        REQUIRE(atomic_data_.at(46).name == "palladium");
        REQUIRE(atomic_data_.at(46).multiplicity == 1);
        REQUIRE(atomic_data_.at(46).mass == Approx(106.4200000000000017));
        REQUIRE(atomic_data_.at(46).covradius == Approx(2.6267193143863810));
        REQUIRE(atomic_data_.at(46).vdwradius == Approx(3.0802535844962597));
        REQUIRE(atomic_data_.at(46).isotopes.at(102).isonum == 102);
        REQUIRE(atomic_data_.at(46).isotopes.at(102).mass == Approx(101.9056000000000068));
        REQUIRE(atomic_data_.at(46).isotopes.at(102).abundance == Approx(0.0102000000000000));
        REQUIRE(atomic_data_.at(46).isotopes.at(104).isonum == 104);
        REQUIRE(atomic_data_.at(46).isotopes.at(104).mass == Approx(103.9040310000000034));
        REQUIRE(atomic_data_.at(46).isotopes.at(104).abundance == Approx(0.1114000000000000));
        REQUIRE(atomic_data_.at(46).isotopes.at(105).isonum == 105);
        REQUIRE(atomic_data_.at(46).isotopes.at(105).mass == Approx(104.9050799999999981));
        REQUIRE(atomic_data_.at(46).isotopes.at(105).abundance == Approx(0.2233000000000000));
        REQUIRE(atomic_data_.at(46).isotopes.at(106).isonum == 106);
        REQUIRE(atomic_data_.at(46).isotopes.at(106).mass == Approx(105.9034800000000018));
        REQUIRE(atomic_data_.at(46).isotopes.at(106).abundance == Approx(0.2733000000000000));
        REQUIRE(atomic_data_.at(46).isotopes.at(108).isonum == 108);
        REQUIRE(atomic_data_.at(46).isotopes.at(108).mass == Approx(107.9038919999999990));
        REQUIRE(atomic_data_.at(46).isotopes.at(108).abundance == Approx(0.2646000000000000));
        REQUIRE(atomic_data_.at(46).isotopes.at(110).isonum == 110);
        REQUIRE(atomic_data_.at(46).isotopes.at(110).mass == Approx(109.9051719999999932));
        REQUIRE(atomic_data_.at(46).isotopes.at(110).abundance == Approx(0.1172000000000000));
    }
    SECTION("Atom # 47")
    {
        REQUIRE(atomic_data_.at(47).Z == 47);
        REQUIRE(atomic_data_.at(47).sym == "ag");
        REQUIRE(atomic_data_.at(47).name == "silver");
        REQUIRE(atomic_data_.at(47).multiplicity == 2);
        REQUIRE(atomic_data_.at(47).mass == Approx(107.8682000000000016));
        REQUIRE(atomic_data_.at(47).covradius == Approx(2.7401028819138507));
        REQUIRE(atomic_data_.at(47).vdwradius == Approx(3.2503289357874645));
        REQUIRE(atomic_data_.at(47).isotopes.at(107).isonum == 107);
        REQUIRE(atomic_data_.at(47).isotopes.at(107).mass == Approx(106.9050900000000013));
        REQUIRE(atomic_data_.at(47).isotopes.at(107).abundance == Approx(0.5183900000000000));
        REQUIRE(atomic_data_.at(47).isotopes.at(109).isonum == 109);
        REQUIRE(atomic_data_.at(47).isotopes.at(109).mass == Approx(108.9047549999999944));
        REQUIRE(atomic_data_.at(47).isotopes.at(109).abundance == Approx(0.4816100000000000));
    }
    SECTION("Atom # 48")
    {
        REQUIRE(atomic_data_.at(48).Z == 48);
        REQUIRE(atomic_data_.at(48).sym == "cd");
        REQUIRE(atomic_data_.at(48).name == "cadmium");
        REQUIRE(atomic_data_.at(48).multiplicity == 1);
        REQUIRE(atomic_data_.at(48).mass == Approx(112.4140000000000015));
        REQUIRE(atomic_data_.at(48).covradius == Approx(2.7212056206592723));
        REQUIRE(atomic_data_.at(48).vdwradius == Approx(2.9857672782233684));
        REQUIRE(atomic_data_.at(48).isotopes.at(106).isonum == 106);
        REQUIRE(atomic_data_.at(48).isotopes.at(106).mass == Approx(105.9064599999999956));
        REQUIRE(atomic_data_.at(48).isotopes.at(106).abundance == Approx(0.0124500000000000));
        REQUIRE(atomic_data_.at(48).isotopes.at(108).isonum == 108);
        REQUIRE(atomic_data_.at(48).isotopes.at(108).mass == Approx(107.9041830000000033));
        REQUIRE(atomic_data_.at(48).isotopes.at(108).abundance == Approx(0.0088800000000000));
        REQUIRE(atomic_data_.at(48).isotopes.at(110).isonum == 110);
        REQUIRE(atomic_data_.at(48).isotopes.at(110).mass == Approx(109.9030070000000023));
        REQUIRE(atomic_data_.at(48).isotopes.at(110).abundance == Approx(0.1247000000000000));
        REQUIRE(atomic_data_.at(48).isotopes.at(111).isonum == 111);
        REQUIRE(atomic_data_.at(48).isotopes.at(111).mass == Approx(110.9041830000000033));
        REQUIRE(atomic_data_.at(48).isotopes.at(111).abundance == Approx(0.1279500000000000));
        REQUIRE(atomic_data_.at(48).isotopes.at(112).isonum == 112);
        REQUIRE(atomic_data_.at(48).isotopes.at(112).mass == Approx(111.9027629999999931));
        REQUIRE(atomic_data_.at(48).isotopes.at(112).abundance == Approx(0.2410900000000000));
        REQUIRE(atomic_data_.at(48).isotopes.at(113).isonum == 113);
        REQUIRE(atomic_data_.at(48).isotopes.at(113).mass == Approx(112.9044080000000037));
        REQUIRE(atomic_data_.at(48).isotopes.at(113).abundance == Approx(0.1222700000000000));
        REQUIRE(atomic_data_.at(48).isotopes.at(114).isonum == 114);
        REQUIRE(atomic_data_.at(48).isotopes.at(114).mass == Approx(113.9033649999999938));
        REQUIRE(atomic_data_.at(48).isotopes.at(114).abundance == Approx(0.2875400000000000));
        REQUIRE(atomic_data_.at(48).isotopes.at(116).isonum == 116);
        REQUIRE(atomic_data_.at(48).isotopes.at(116).mass == Approx(115.9047630000000026));
        REQUIRE(atomic_data_.at(48).isotopes.at(116).abundance == Approx(0.0751200000000000));
    }
    SECTION("Atom # 49")
    {
        REQUIRE(atomic_data_.at(49).Z == 49);
        REQUIRE(atomic_data_.at(49).sym == "in");
        REQUIRE(atomic_data_.at(49).name == "indium");
        REQUIRE(atomic_data_.at(49).multiplicity == 2);
        REQUIRE(atomic_data_.at(49).mass == Approx(114.8179999999999978));
        REQUIRE(atomic_data_.at(49).covradius == Approx(2.6834110981501160));
        REQUIRE(atomic_data_.at(49).vdwradius == Approx(3.6471714221336082));
        REQUIRE(atomic_data_.at(49).isotopes.at(113).isonum == 113);
        REQUIRE(atomic_data_.at(49).isotopes.at(113).mass == Approx(112.9040619999999961));
        REQUIRE(atomic_data_.at(49).isotopes.at(113).abundance == Approx(0.0428100000000000));
        REQUIRE(atomic_data_.at(49).isotopes.at(115).isonum == 115);
        REQUIRE(atomic_data_.at(49).isotopes.at(115).mass == Approx(114.9038787799999994));
        REQUIRE(atomic_data_.at(49).isotopes.at(115).abundance == Approx(0.9571900000000000));
    }
    SECTION("Atom # 50")
    {
        REQUIRE(atomic_data_.at(50).Z == 50);
        REQUIRE(atomic_data_.at(50).sym == "sn");
        REQUIRE(atomic_data_.at(50).name == "tin");
        REQUIRE(atomic_data_.at(50).multiplicity == 3);
        REQUIRE(atomic_data_.at(50).mass == Approx(118.7099999999999937));
        REQUIRE(atomic_data_.at(50).covradius == Approx(2.6267193143863810));
        REQUIRE(atomic_data_.at(50).vdwradius == Approx(4.1007056922434870));
        REQUIRE(atomic_data_.at(50).isotopes.at(112).isonum == 112);
        REQUIRE(atomic_data_.at(50).isotopes.at(112).mass == Approx(111.9048240000000050));
        REQUIRE(atomic_data_.at(50).isotopes.at(112).abundance == Approx(0.0097000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(114).isonum == 114);
        REQUIRE(atomic_data_.at(50).isotopes.at(114).mass == Approx(113.9027829999999994));
        REQUIRE(atomic_data_.at(50).isotopes.at(114).abundance == Approx(0.0066000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(115).isonum == 115);
        REQUIRE(atomic_data_.at(50).isotopes.at(115).mass == Approx(114.9033447000000052));
        REQUIRE(atomic_data_.at(50).isotopes.at(115).abundance == Approx(0.0034000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(116).isonum == 116);
        REQUIRE(atomic_data_.at(50).isotopes.at(116).mass == Approx(115.9017429999999962));
        REQUIRE(atomic_data_.at(50).isotopes.at(116).abundance == Approx(0.1454000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(117).isonum == 117);
        REQUIRE(atomic_data_.at(50).isotopes.at(117).mass == Approx(116.9029539999999940));
        REQUIRE(atomic_data_.at(50).isotopes.at(117).abundance == Approx(0.0768000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(118).isonum == 118);
        REQUIRE(atomic_data_.at(50).isotopes.at(118).mass == Approx(117.9016069999999985));
        REQUIRE(atomic_data_.at(50).isotopes.at(118).abundance == Approx(0.2422000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(119).isonum == 119);
        REQUIRE(atomic_data_.at(50).isotopes.at(119).mass == Approx(118.9033110000000022));
        REQUIRE(atomic_data_.at(50).isotopes.at(119).abundance == Approx(0.0859000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(120).isonum == 120);
        REQUIRE(atomic_data_.at(50).isotopes.at(120).mass == Approx(119.9022020000000026));
        REQUIRE(atomic_data_.at(50).isotopes.at(120).abundance == Approx(0.3258000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(122).isonum == 122);
        REQUIRE(atomic_data_.at(50).isotopes.at(122).mass == Approx(121.9034400000000034));
        REQUIRE(atomic_data_.at(50).isotopes.at(122).abundance == Approx(0.0463000000000000));
        REQUIRE(atomic_data_.at(50).isotopes.at(124).isonum == 124);
        REQUIRE(atomic_data_.at(50).isotopes.at(124).mass == Approx(123.9052769999999981));
        REQUIRE(atomic_data_.at(50).isotopes.at(124).abundance == Approx(0.0579000000000000));
    }
    SECTION("Atom # 51")
    {
        REQUIRE(atomic_data_.at(51).Z == 51);
        REQUIRE(atomic_data_.at(51).sym == "sb");
        REQUIRE(atomic_data_.at(51).name == "antimony");
        REQUIRE(atomic_data_.at(51).multiplicity == 4);
        REQUIRE(atomic_data_.at(51).mass == Approx(121.7600000000000051));
        REQUIRE(atomic_data_.at(51).covradius == Approx(2.6267193143863810));
        REQUIRE(atomic_data_.at(51).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(51).isotopes.at(121).isonum == 121);
        REQUIRE(atomic_data_.at(51).isotopes.at(121).mass == Approx(120.9038099999999929));
        REQUIRE(atomic_data_.at(51).isotopes.at(121).abundance == Approx(0.5721000000000001));
        REQUIRE(atomic_data_.at(51).isotopes.at(123).isonum == 123);
        REQUIRE(atomic_data_.at(51).isotopes.at(123).mass == Approx(122.9042100000000062));
        REQUIRE(atomic_data_.at(51).isotopes.at(123).abundance == Approx(0.4279000000000000));
    }
    SECTION("Atom # 52")
    {
        REQUIRE(atomic_data_.at(52).Z == 52);
        REQUIRE(atomic_data_.at(52).sym == "te");
        REQUIRE(atomic_data_.at(52).name == "tellurium");
        REQUIRE(atomic_data_.at(52).multiplicity == 3);
        REQUIRE(atomic_data_.at(52).mass == Approx(127.5999999999999943));
        REQUIRE(atomic_data_.at(52).covradius == Approx(2.6078220531318026));
        REQUIRE(atomic_data_.at(52).vdwradius == Approx(3.8928358184431260));
        REQUIRE(atomic_data_.at(52).isotopes.at(120).isonum == 120);
        REQUIRE(atomic_data_.at(52).isotopes.at(120).mass == Approx(119.9040600000000012));
        REQUIRE(atomic_data_.at(52).isotopes.at(120).abundance == Approx(0.0009000000000000));
        REQUIRE(atomic_data_.at(52).isotopes.at(122).isonum == 122);
        REQUIRE(atomic_data_.at(52).isotopes.at(122).mass == Approx(121.9030400000000043));
        REQUIRE(atomic_data_.at(52).isotopes.at(122).abundance == Approx(0.0255000000000000));
        REQUIRE(atomic_data_.at(52).isotopes.at(123).isonum == 123);
        REQUIRE(atomic_data_.at(52).isotopes.at(123).mass == Approx(122.9042699999999968));
        REQUIRE(atomic_data_.at(52).isotopes.at(123).abundance == Approx(0.0089000000000000));
        REQUIRE(atomic_data_.at(52).isotopes.at(124).isonum == 124);
        REQUIRE(atomic_data_.at(52).isotopes.at(124).mass == Approx(123.9028200000000055));
        REQUIRE(atomic_data_.at(52).isotopes.at(124).abundance == Approx(0.0474000000000000));
        REQUIRE(atomic_data_.at(52).isotopes.at(125).isonum == 125);
        REQUIRE(atomic_data_.at(52).isotopes.at(125).mass == Approx(124.9044300000000050));
        REQUIRE(atomic_data_.at(52).isotopes.at(125).abundance == Approx(0.0707000000000000));
        REQUIRE(atomic_data_.at(52).isotopes.at(126).isonum == 126);
        REQUIRE(atomic_data_.at(52).isotopes.at(126).mass == Approx(125.9033100000000047));
        REQUIRE(atomic_data_.at(52).isotopes.at(126).abundance == Approx(0.1884000000000000));
        REQUIRE(atomic_data_.at(52).isotopes.at(128).isonum == 128);
        REQUIRE(atomic_data_.at(52).isotopes.at(128).mass == Approx(127.9044609999999977));
        REQUIRE(atomic_data_.at(52).isotopes.at(128).abundance == Approx(0.3174000000000000));
        REQUIRE(atomic_data_.at(52).isotopes.at(130).isonum == 130);
        REQUIRE(atomic_data_.at(52).isotopes.at(130).mass == Approx(129.9062227500000120));
        REQUIRE(atomic_data_.at(52).isotopes.at(130).abundance == Approx(0.3408000000000000));
    }
    SECTION("Atom # 53")
    {
        REQUIRE(atomic_data_.at(53).Z == 53);
        REQUIRE(atomic_data_.at(53).sym == "i");
        REQUIRE(atomic_data_.at(53).name == "iodine");
        REQUIRE(atomic_data_.at(53).multiplicity == 2);
        REQUIRE(atomic_data_.at(53).mass == Approx(126.9044700000000034));
        REQUIRE(atomic_data_.at(53).covradius == Approx(2.6267193143863810));
        REQUIRE(atomic_data_.at(53).vdwradius == Approx(3.7416577284064996));
        REQUIRE(atomic_data_.at(53).isotopes.at(127).isonum == 127);
        REQUIRE(atomic_data_.at(53).isotopes.at(127).mass == Approx(126.9044700000000034));
        REQUIRE(atomic_data_.at(53).isotopes.at(127).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 54")
    {
        REQUIRE(atomic_data_.at(54).Z == 54);
        REQUIRE(atomic_data_.at(54).sym == "xe");
        REQUIRE(atomic_data_.at(54).name == "xenon");
        REQUIRE(atomic_data_.at(54).multiplicity == 1);
        REQUIRE(atomic_data_.at(54).mass == Approx(131.2930000000000064));
        REQUIRE(atomic_data_.at(54).covradius == Approx(2.6456165756409593));
        REQUIRE(atomic_data_.at(54).vdwradius == Approx(4.0818084309889091));
        REQUIRE(atomic_data_.at(54).isotopes.at(124).isonum == 124);
        REQUIRE(atomic_data_.at(54).isotopes.at(124).mass == Approx(123.9058899999999994));
        REQUIRE(atomic_data_.at(54).isotopes.at(124).abundance == Approx(0.0009500000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(126).isonum == 126);
        REQUIRE(atomic_data_.at(54).isotopes.at(126).mass == Approx(125.9043000000000063));
        REQUIRE(atomic_data_.at(54).isotopes.at(126).abundance == Approx(0.0008900000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(128).isonum == 128);
        REQUIRE(atomic_data_.at(54).isotopes.at(128).mass == Approx(127.9035310000000010));
        REQUIRE(atomic_data_.at(54).isotopes.at(128).abundance == Approx(0.0191000000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(129).isonum == 129);
        REQUIRE(atomic_data_.at(54).isotopes.at(129).mass == Approx(128.9047808599999883));
        REQUIRE(atomic_data_.at(54).isotopes.at(129).abundance == Approx(0.2640100000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(130).isonum == 130);
        REQUIRE(atomic_data_.at(54).isotopes.at(130).mass == Approx(129.9035093999999901));
        REQUIRE(atomic_data_.at(54).isotopes.at(130).abundance == Approx(0.0407100000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(131).isonum == 131);
        REQUIRE(atomic_data_.at(54).isotopes.at(131).mass == Approx(130.9050839999999880));
        REQUIRE(atomic_data_.at(54).isotopes.at(131).abundance == Approx(0.2123200000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(132).isonum == 132);
        REQUIRE(atomic_data_.at(54).isotopes.at(132).mass == Approx(131.9041550899999891));
        REQUIRE(atomic_data_.at(54).isotopes.at(132).abundance == Approx(0.2690900000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(134).isonum == 134);
        REQUIRE(atomic_data_.at(54).isotopes.at(134).mass == Approx(133.9053949999999986));
        REQUIRE(atomic_data_.at(54).isotopes.at(134).abundance == Approx(0.1043600000000000));
        REQUIRE(atomic_data_.at(54).isotopes.at(136).isonum == 136);
        REQUIRE(atomic_data_.at(54).isotopes.at(136).mass == Approx(135.9072144799999933));
        REQUIRE(atomic_data_.at(54).isotopes.at(136).abundance == Approx(0.0885700000000000));
    }
    SECTION("Atom # 55")
    {
        REQUIRE(atomic_data_.at(55).Z == 55);
        REQUIRE(atomic_data_.at(55).sym == "cs");
        REQUIRE(atomic_data_.at(55).name == "cesium");
        REQUIRE(atomic_data_.at(55).multiplicity == 2);
        REQUIRE(atomic_data_.at(55).mass == Approx(132.9054519599999935));
        REQUIRE(atomic_data_.at(55).covradius == Approx(4.6109317461171004));
        REQUIRE(atomic_data_.at(55).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(55).isotopes.at(133).isonum == 133);
        REQUIRE(atomic_data_.at(55).isotopes.at(133).mass == Approx(132.9054519599999935));
        REQUIRE(atomic_data_.at(55).isotopes.at(133).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 56")
    {
        REQUIRE(atomic_data_.at(56).Z == 56);
        REQUIRE(atomic_data_.at(56).sym == "ba");
        REQUIRE(atomic_data_.at(56).name == "barium");
        REQUIRE(atomic_data_.at(56).multiplicity == 1);
        REQUIRE(atomic_data_.at(56).mass == Approx(137.3269999999999982));
        REQUIRE(atomic_data_.at(56).covradius == Approx(4.0629111697343303));
        REQUIRE(atomic_data_.at(56).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(56).isotopes.at(130).isonum == 130);
        REQUIRE(atomic_data_.at(56).isotopes.at(130).mass == Approx(129.9063199999999938));
        REQUIRE(atomic_data_.at(56).isotopes.at(130).abundance == Approx(0.0011000000000000));
        REQUIRE(atomic_data_.at(56).isotopes.at(132).isonum == 132);
        REQUIRE(atomic_data_.at(56).isotopes.at(132).mass == Approx(131.9050609999999892));
        REQUIRE(atomic_data_.at(56).isotopes.at(132).abundance == Approx(0.0010000000000000));
        REQUIRE(atomic_data_.at(56).isotopes.at(134).isonum == 134);
        REQUIRE(atomic_data_.at(56).isotopes.at(134).mass == Approx(133.9045079999999928));
        REQUIRE(atomic_data_.at(56).isotopes.at(134).abundance == Approx(0.0242000000000000));
        REQUIRE(atomic_data_.at(56).isotopes.at(135).isonum == 135);
        REQUIRE(atomic_data_.at(56).isotopes.at(135).mass == Approx(134.9056879999999978));
        REQUIRE(atomic_data_.at(56).isotopes.at(135).abundance == Approx(0.0659000000000000));
        REQUIRE(atomic_data_.at(56).isotopes.at(136).isonum == 136);
        REQUIRE(atomic_data_.at(56).isotopes.at(136).mass == Approx(135.9045759999999916));
        REQUIRE(atomic_data_.at(56).isotopes.at(136).abundance == Approx(0.0785000000000000));
        REQUIRE(atomic_data_.at(56).isotopes.at(137).isonum == 137);
        REQUIRE(atomic_data_.at(56).isotopes.at(137).mass == Approx(136.9058269999999879));
        REQUIRE(atomic_data_.at(56).isotopes.at(137).abundance == Approx(0.1123000000000000));
        REQUIRE(atomic_data_.at(56).isotopes.at(138).isonum == 138);
        REQUIRE(atomic_data_.at(56).isotopes.at(138).mass == Approx(137.9052470000000028));
        REQUIRE(atomic_data_.at(56).isotopes.at(138).abundance == Approx(0.7170000000000000));
    }
    SECTION("Atom # 57")
    {
        REQUIRE(atomic_data_.at(57).Z == 57);
        REQUIRE(atomic_data_.at(57).sym == "la");
        REQUIRE(atomic_data_.at(57).name == "lanthanum");
        REQUIRE(atomic_data_.at(57).multiplicity == 2);
        REQUIRE(atomic_data_.at(57).mass == Approx(138.9054700000000082));
        REQUIRE(atomic_data_.at(57).covradius == Approx(3.9117330796977043));
        REQUIRE(atomic_data_.at(57).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(57).isotopes.at(138).isonum == 138);
        REQUIRE(atomic_data_.at(57).isotopes.at(138).mass == Approx(137.9071199999999919));
        REQUIRE(atomic_data_.at(57).isotopes.at(138).abundance == Approx(0.0008881000000000));
        REQUIRE(atomic_data_.at(57).isotopes.at(139).isonum == 139);
        REQUIRE(atomic_data_.at(57).isotopes.at(139).mass == Approx(138.9063600000000065));
        REQUIRE(atomic_data_.at(57).isotopes.at(139).abundance == Approx(0.9991119000000001));
    }
    SECTION("Atom # 58")
    {
        REQUIRE(atomic_data_.at(58).Z == 58);
        REQUIRE(atomic_data_.at(58).sym == "ce");
        REQUIRE(atomic_data_.at(58).name == "cerium");
        REQUIRE(atomic_data_.at(58).multiplicity == 1);
        REQUIRE(atomic_data_.at(58).mass == Approx(140.1160000000000139));
        REQUIRE(atomic_data_.at(58).covradius == Approx(3.8550412959339693));
        REQUIRE(atomic_data_.at(58).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(58).isotopes.at(136).isonum == 136);
        REQUIRE(atomic_data_.at(58).isotopes.at(136).mass == Approx(135.9071289999999976));
        REQUIRE(atomic_data_.at(58).isotopes.at(136).abundance == Approx(0.0018600000000000));
        REQUIRE(atomic_data_.at(58).isotopes.at(138).isonum == 138);
        REQUIRE(atomic_data_.at(58).isotopes.at(138).mass == Approx(137.9059900000000027));
        REQUIRE(atomic_data_.at(58).isotopes.at(138).abundance == Approx(0.0025100000000000));
        REQUIRE(atomic_data_.at(58).isotopes.at(140).isonum == 140);
        REQUIRE(atomic_data_.at(58).isotopes.at(140).mass == Approx(139.9054399999999987));
        REQUIRE(atomic_data_.at(58).isotopes.at(140).abundance == Approx(0.8844900000000000));
        REQUIRE(atomic_data_.at(58).isotopes.at(142).isonum == 142);
        REQUIRE(atomic_data_.at(58).isotopes.at(142).mass == Approx(141.9092499999999859));
        REQUIRE(atomic_data_.at(58).isotopes.at(142).abundance == Approx(0.1111400000000000));
    }
    SECTION("Atom # 59")
    {
        REQUIRE(atomic_data_.at(59).Z == 59);
        REQUIRE(atomic_data_.at(59).sym == "pr");
        REQUIRE(atomic_data_.at(59).name == "praseodymium");
        REQUIRE(atomic_data_.at(59).multiplicity == 4);
        REQUIRE(atomic_data_.at(59).mass == Approx(140.9076599999999928));
        REQUIRE(atomic_data_.at(59).covradius == Approx(3.8361440346793909));
        REQUIRE(atomic_data_.at(59).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(59).isotopes.at(141).isonum == 141);
        REQUIRE(atomic_data_.at(59).isotopes.at(141).mass == Approx(140.9076599999999928));
        REQUIRE(atomic_data_.at(59).isotopes.at(141).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 60")
    {
        REQUIRE(atomic_data_.at(60).Z == 60);
        REQUIRE(atomic_data_.at(60).sym == "nd");
        REQUIRE(atomic_data_.at(60).name == "neodymium");
        REQUIRE(atomic_data_.at(60).multiplicity == 5);
        REQUIRE(atomic_data_.at(60).mass == Approx(144.2419999999999902));
        REQUIRE(atomic_data_.at(60).covradius == Approx(3.7983495121702346));
        REQUIRE(atomic_data_.at(60).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(60).isotopes.at(142).isonum == 142);
        REQUIRE(atomic_data_.at(60).isotopes.at(142).mass == Approx(141.9077299999999866));
        REQUIRE(atomic_data_.at(60).isotopes.at(142).abundance == Approx(0.2715300000000000));
        REQUIRE(atomic_data_.at(60).isotopes.at(143).isonum == 143);
        REQUIRE(atomic_data_.at(60).isotopes.at(143).mass == Approx(142.9098199999999963));
        REQUIRE(atomic_data_.at(60).isotopes.at(143).abundance == Approx(0.1217300000000000));
        REQUIRE(atomic_data_.at(60).isotopes.at(144).isonum == 144);
        REQUIRE(atomic_data_.at(60).isotopes.at(144).mass == Approx(143.9100899999999967));
        REQUIRE(atomic_data_.at(60).isotopes.at(144).abundance == Approx(0.2379800000000000));
        REQUIRE(atomic_data_.at(60).isotopes.at(145).isonum == 145);
        REQUIRE(atomic_data_.at(60).isotopes.at(145).mass == Approx(144.9125799999999913));
        REQUIRE(atomic_data_.at(60).isotopes.at(145).abundance == Approx(0.0829300000000000));
        REQUIRE(atomic_data_.at(60).isotopes.at(146).isonum == 146);
        REQUIRE(atomic_data_.at(60).isotopes.at(146).mass == Approx(145.9131199999999922));
        REQUIRE(atomic_data_.at(60).isotopes.at(146).abundance == Approx(0.1718900000000000));
        REQUIRE(atomic_data_.at(60).isotopes.at(148).isonum == 148);
        REQUIRE(atomic_data_.at(60).isotopes.at(148).mass == Approx(147.9168999999999983));
        REQUIRE(atomic_data_.at(60).isotopes.at(148).abundance == Approx(0.0575600000000000));
        REQUIRE(atomic_data_.at(60).isotopes.at(150).isonum == 150);
        REQUIRE(atomic_data_.at(60).isotopes.at(150).mass == Approx(149.9208999999999889));
        REQUIRE(atomic_data_.at(60).isotopes.at(150).abundance == Approx(0.0563800000000000));
    }
    SECTION("Atom # 61")
    {
        REQUIRE(atomic_data_.at(61).Z == 61);
        REQUIRE(atomic_data_.at(61).sym == "pm");
        REQUIRE(atomic_data_.at(61).name == "promethium");
        REQUIRE(atomic_data_.at(61).multiplicity == 6);
        REQUIRE(atomic_data_.at(61).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(61).covradius == Approx(3.7605549896610779));
        REQUIRE(atomic_data_.at(61).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(61).isotopes.at(145).isonum == 145);
        REQUIRE(atomic_data_.at(61).isotopes.at(145).mass == Approx(144.9127599999999916));
        REQUIRE(atomic_data_.at(61).isotopes.at(145).abundance == Approx(0.0000000000000000));
    }
    SECTION("Atom # 62")
    {
        REQUIRE(atomic_data_.at(62).Z == 62);
        REQUIRE(atomic_data_.at(62).sym == "sm");
        REQUIRE(atomic_data_.at(62).name == "samarium");
        REQUIRE(atomic_data_.at(62).multiplicity == 7);
        REQUIRE(atomic_data_.at(62).mass == Approx(150.3600000000000136));
        REQUIRE(atomic_data_.at(62).covradius == Approx(3.7416577284064996));
        REQUIRE(atomic_data_.at(62).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(62).isotopes.at(144).isonum == 144);
        REQUIRE(atomic_data_.at(62).isotopes.at(144).mass == Approx(143.9120100000000093));
        REQUIRE(atomic_data_.at(62).isotopes.at(144).abundance == Approx(0.0308000000000000));
        REQUIRE(atomic_data_.at(62).isotopes.at(147).isonum == 147);
        REQUIRE(atomic_data_.at(62).isotopes.at(147).mass == Approx(146.9148999999999887));
        REQUIRE(atomic_data_.at(62).isotopes.at(147).abundance == Approx(0.1500000000000000));
        REQUIRE(atomic_data_.at(62).isotopes.at(148).isonum == 148);
        REQUIRE(atomic_data_.at(62).isotopes.at(148).mass == Approx(147.9148299999999949));
        REQUIRE(atomic_data_.at(62).isotopes.at(148).abundance == Approx(0.1125000000000000));
        REQUIRE(atomic_data_.at(62).isotopes.at(149).isonum == 149);
        REQUIRE(atomic_data_.at(62).isotopes.at(149).mass == Approx(148.9171900000000051));
        REQUIRE(atomic_data_.at(62).isotopes.at(149).abundance == Approx(0.1382000000000000));
        REQUIRE(atomic_data_.at(62).isotopes.at(150).isonum == 150);
        REQUIRE(atomic_data_.at(62).isotopes.at(150).mass == Approx(149.9172800000000052));
        REQUIRE(atomic_data_.at(62).isotopes.at(150).abundance == Approx(0.0737000000000000));
        REQUIRE(atomic_data_.at(62).isotopes.at(152).isonum == 152);
        REQUIRE(atomic_data_.at(62).isotopes.at(152).mass == Approx(151.9197399999999902));
        REQUIRE(atomic_data_.at(62).isotopes.at(152).abundance == Approx(0.2674000000000000));
        REQUIRE(atomic_data_.at(62).isotopes.at(154).isonum == 154);
        REQUIRE(atomic_data_.at(62).isotopes.at(154).mass == Approx(153.9222200000000100));
        REQUIRE(atomic_data_.at(62).isotopes.at(154).abundance == Approx(0.2274000000000000));
    }
    SECTION("Atom # 63")
    {
        REQUIRE(atomic_data_.at(63).Z == 63);
        REQUIRE(atomic_data_.at(63).sym == "eu");
        REQUIRE(atomic_data_.at(63).name == "europium");
        REQUIRE(atomic_data_.at(63).multiplicity == 8);
        REQUIRE(atomic_data_.at(63).mass == Approx(151.9639999999999986));
        REQUIRE(atomic_data_.at(63).covradius == Approx(3.7416577284064996));
        REQUIRE(atomic_data_.at(63).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(63).isotopes.at(151).isonum == 151);
        REQUIRE(atomic_data_.at(63).isotopes.at(151).mass == Approx(150.9198599999999999));
        REQUIRE(atomic_data_.at(63).isotopes.at(151).abundance == Approx(0.4781000000000000));
        REQUIRE(atomic_data_.at(63).isotopes.at(153).isonum == 153);
        REQUIRE(atomic_data_.at(63).isotopes.at(153).mass == Approx(152.9212400000000116));
        REQUIRE(atomic_data_.at(63).isotopes.at(153).abundance == Approx(0.5219000000000000));
    }
    SECTION("Atom # 64")
    {
        REQUIRE(atomic_data_.at(64).Z == 64);
        REQUIRE(atomic_data_.at(64).sym == "gd");
        REQUIRE(atomic_data_.at(64).name == "gadolinium");
        REQUIRE(atomic_data_.at(64).multiplicity == 9);
        REQUIRE(atomic_data_.at(64).mass == Approx(157.2500000000000000));
        REQUIRE(atomic_data_.at(64).covradius == Approx(3.7038632058973433));
        REQUIRE(atomic_data_.at(64).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(64).isotopes.at(152).isonum == 152);
        REQUIRE(atomic_data_.at(64).isotopes.at(152).mass == Approx(151.9198000000000093));
        REQUIRE(atomic_data_.at(64).isotopes.at(152).abundance == Approx(0.0020000000000000));
        REQUIRE(atomic_data_.at(64).isotopes.at(154).isonum == 154);
        REQUIRE(atomic_data_.at(64).isotopes.at(154).mass == Approx(153.9208700000000078));
        REQUIRE(atomic_data_.at(64).isotopes.at(154).abundance == Approx(0.0218000000000000));
        REQUIRE(atomic_data_.at(64).isotopes.at(155).isonum == 155);
        REQUIRE(atomic_data_.at(64).isotopes.at(155).mass == Approx(154.9226299999999981));
        REQUIRE(atomic_data_.at(64).isotopes.at(155).abundance == Approx(0.1480000000000000));
        REQUIRE(atomic_data_.at(64).isotopes.at(156).isonum == 156);
        REQUIRE(atomic_data_.at(64).isotopes.at(156).mass == Approx(155.9221300000000099));
        REQUIRE(atomic_data_.at(64).isotopes.at(156).abundance == Approx(0.2047000000000000));
        REQUIRE(atomic_data_.at(64).isotopes.at(157).isonum == 157);
        REQUIRE(atomic_data_.at(64).isotopes.at(157).mass == Approx(156.9239699999999971));
        REQUIRE(atomic_data_.at(64).isotopes.at(157).abundance == Approx(0.1565000000000000));
        REQUIRE(atomic_data_.at(64).isotopes.at(158).isonum == 158);
        REQUIRE(atomic_data_.at(64).isotopes.at(158).mass == Approx(157.9241100000000131));
        REQUIRE(atomic_data_.at(64).isotopes.at(158).abundance == Approx(0.2484000000000000));
        REQUIRE(atomic_data_.at(64).isotopes.at(160).isonum == 160);
        REQUIRE(atomic_data_.at(64).isotopes.at(160).mass == Approx(159.9270600000000115));
        REQUIRE(atomic_data_.at(64).isotopes.at(160).abundance == Approx(0.2186000000000000));
    }
    SECTION("Atom # 65")
    {
        REQUIRE(atomic_data_.at(65).Z == 65);
        REQUIRE(atomic_data_.at(65).sym == "tb");
        REQUIRE(atomic_data_.at(65).name == "terbium");
        REQUIRE(atomic_data_.at(65).multiplicity == 6);
        REQUIRE(atomic_data_.at(65).mass == Approx(158.9253500000000088));
        REQUIRE(atomic_data_.at(65).covradius == Approx(3.6660686833881866));
        REQUIRE(atomic_data_.at(65).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(65).isotopes.at(159).isonum == 159);
        REQUIRE(atomic_data_.at(65).isotopes.at(159).mass == Approx(158.9253500000000088));
        REQUIRE(atomic_data_.at(65).isotopes.at(159).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 66")
    {
        REQUIRE(atomic_data_.at(66).Z == 66);
        REQUIRE(atomic_data_.at(66).sym == "dy");
        REQUIRE(atomic_data_.at(66).name == "dysprosium");
        REQUIRE(atomic_data_.at(66).multiplicity == 5);
        REQUIRE(atomic_data_.at(66).mass == Approx(162.5000000000000000));
        REQUIRE(atomic_data_.at(66).covradius == Approx(3.6282741608790299));
        REQUIRE(atomic_data_.at(66).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(66).isotopes.at(156).isonum == 156);
        REQUIRE(atomic_data_.at(66).isotopes.at(156).mass == Approx(155.9242800000000102));
        REQUIRE(atomic_data_.at(66).isotopes.at(156).abundance == Approx(0.0005600000000000));
        REQUIRE(atomic_data_.at(66).isotopes.at(158).isonum == 158);
        REQUIRE(atomic_data_.at(66).isotopes.at(158).mass == Approx(157.9244199999999978));
        REQUIRE(atomic_data_.at(66).isotopes.at(158).abundance == Approx(0.0009500000000000));
        REQUIRE(atomic_data_.at(66).isotopes.at(160).isonum == 160);
        REQUIRE(atomic_data_.at(66).isotopes.at(160).mass == Approx(159.9251999999999896));
        REQUIRE(atomic_data_.at(66).isotopes.at(160).abundance == Approx(0.0232900000000000));
        REQUIRE(atomic_data_.at(66).isotopes.at(161).isonum == 161);
        REQUIRE(atomic_data_.at(66).isotopes.at(161).mass == Approx(160.9269400000000019));
        REQUIRE(atomic_data_.at(66).isotopes.at(161).abundance == Approx(0.1888900000000000));
        REQUIRE(atomic_data_.at(66).isotopes.at(162).isonum == 162);
        REQUIRE(atomic_data_.at(66).isotopes.at(162).mass == Approx(161.9268099999999890));
        REQUIRE(atomic_data_.at(66).isotopes.at(162).abundance == Approx(0.2547500000000000));
        REQUIRE(atomic_data_.at(66).isotopes.at(163).isonum == 163);
        REQUIRE(atomic_data_.at(66).isotopes.at(163).mass == Approx(162.9287400000000048));
        REQUIRE(atomic_data_.at(66).isotopes.at(163).abundance == Approx(0.2489600000000000));
        REQUIRE(atomic_data_.at(66).isotopes.at(164).isonum == 164);
        REQUIRE(atomic_data_.at(66).isotopes.at(164).mass == Approx(163.9291800000000023));
        REQUIRE(atomic_data_.at(66).isotopes.at(164).abundance == Approx(0.2826000000000000));
    }
    SECTION("Atom # 67")
    {
        REQUIRE(atomic_data_.at(67).Z == 67);
        REQUIRE(atomic_data_.at(67).sym == "ho");
        REQUIRE(atomic_data_.at(67).name == "holmium");
        REQUIRE(atomic_data_.at(67).multiplicity == 4);
        REQUIRE(atomic_data_.at(67).mass == Approx(164.9303299999999979));
        REQUIRE(atomic_data_.at(67).covradius == Approx(3.6282741608790299));
        REQUIRE(atomic_data_.at(67).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(67).isotopes.at(165).isonum == 165);
        REQUIRE(atomic_data_.at(67).isotopes.at(165).mass == Approx(164.9303299999999979));
        REQUIRE(atomic_data_.at(67).isotopes.at(165).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 68")
    {
        REQUIRE(atomic_data_.at(68).Z == 68);
        REQUIRE(atomic_data_.at(68).sym == "er");
        REQUIRE(atomic_data_.at(68).name == "erbium");
        REQUIRE(atomic_data_.at(68).multiplicity == 3);
        REQUIRE(atomic_data_.at(68).mass == Approx(167.2589999999999861));
        REQUIRE(atomic_data_.at(68).covradius == Approx(3.5715823771152952));
        REQUIRE(atomic_data_.at(68).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(68).isotopes.at(162).isonum == 162);
        REQUIRE(atomic_data_.at(68).isotopes.at(162).mass == Approx(161.9287899999999922));
        REQUIRE(atomic_data_.at(68).isotopes.at(162).abundance == Approx(0.0013900000000000));
        REQUIRE(atomic_data_.at(68).isotopes.at(164).isonum == 164);
        REQUIRE(atomic_data_.at(68).isotopes.at(164).mass == Approx(163.9292100000000119));
        REQUIRE(atomic_data_.at(68).isotopes.at(164).abundance == Approx(0.0160100000000000));
        REQUIRE(atomic_data_.at(68).isotopes.at(166).isonum == 166);
        REQUIRE(atomic_data_.at(68).isotopes.at(166).mass == Approx(165.9302999999999884));
        REQUIRE(atomic_data_.at(68).isotopes.at(166).abundance == Approx(0.3350300000000000));
        REQUIRE(atomic_data_.at(68).isotopes.at(167).isonum == 167);
        REQUIRE(atomic_data_.at(68).isotopes.at(167).mass == Approx(166.9320500000000038));
        REQUIRE(atomic_data_.at(68).isotopes.at(167).abundance == Approx(0.2286900000000000));
        REQUIRE(atomic_data_.at(68).isotopes.at(168).isonum == 168);
        REQUIRE(atomic_data_.at(68).isotopes.at(168).mass == Approx(167.9323799999999949));
        REQUIRE(atomic_data_.at(68).isotopes.at(168).abundance == Approx(0.2697800000000000));
        REQUIRE(atomic_data_.at(68).isotopes.at(170).isonum == 170);
        REQUIRE(atomic_data_.at(68).isotopes.at(170).mass == Approx(169.9354700000000093));
        REQUIRE(atomic_data_.at(68).isotopes.at(170).abundance == Approx(0.1491000000000000));
    }
    SECTION("Atom # 69")
    {
        REQUIRE(atomic_data_.at(69).Z == 69);
        REQUIRE(atomic_data_.at(69).sym == "tm");
        REQUIRE(atomic_data_.at(69).name == "thulium");
        REQUIRE(atomic_data_.at(69).multiplicity == 2);
        REQUIRE(atomic_data_.at(69).mass == Approx(168.9342200000000105));
        REQUIRE(atomic_data_.at(69).covradius == Approx(3.5904796383698736));
        REQUIRE(atomic_data_.at(69).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(69).isotopes.at(169).isonum == 169);
        REQUIRE(atomic_data_.at(69).isotopes.at(169).mass == Approx(168.9342200000000105));
        REQUIRE(atomic_data_.at(69).isotopes.at(169).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 70")
    {
        REQUIRE(atomic_data_.at(70).Z == 70);
        REQUIRE(atomic_data_.at(70).sym == "yb");
        REQUIRE(atomic_data_.at(70).name == "ytterbium");
        REQUIRE(atomic_data_.at(70).multiplicity == 1);
        REQUIRE(atomic_data_.at(70).mass == Approx(173.0449999999999875));
        REQUIRE(atomic_data_.at(70).covradius == Approx(3.5337878546061385));
        REQUIRE(atomic_data_.at(70).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(70).isotopes.at(168).isonum == 168);
        REQUIRE(atomic_data_.at(70).isotopes.at(168).mass == Approx(167.9338899999999910));
        REQUIRE(atomic_data_.at(70).isotopes.at(168).abundance == Approx(0.0012600000000000));
        REQUIRE(atomic_data_.at(70).isotopes.at(170).isonum == 170);
        REQUIRE(atomic_data_.at(70).isotopes.at(170).mass == Approx(169.9347699999999861));
        REQUIRE(atomic_data_.at(70).isotopes.at(170).abundance == Approx(0.0302300000000000));
        REQUIRE(atomic_data_.at(70).isotopes.at(171).isonum == 171);
        REQUIRE(atomic_data_.at(70).isotopes.at(171).mass == Approx(170.9363299999999981));
        REQUIRE(atomic_data_.at(70).isotopes.at(171).abundance == Approx(0.1421600000000000));
        REQUIRE(atomic_data_.at(70).isotopes.at(172).isonum == 172);
        REQUIRE(atomic_data_.at(70).isotopes.at(172).mass == Approx(171.9363899999999887));
        REQUIRE(atomic_data_.at(70).isotopes.at(172).abundance == Approx(0.2175400000000000));
        REQUIRE(atomic_data_.at(70).isotopes.at(173).isonum == 173);
        REQUIRE(atomic_data_.at(70).isotopes.at(173).mass == Approx(172.9382200000000012));
        REQUIRE(atomic_data_.at(70).isotopes.at(173).abundance == Approx(0.1609800000000000));
        REQUIRE(atomic_data_.at(70).isotopes.at(174).isonum == 174);
        REQUIRE(atomic_data_.at(70).isotopes.at(174).mass == Approx(173.9388700000000085));
        REQUIRE(atomic_data_.at(70).isotopes.at(174).abundance == Approx(0.3189600000000000));
        REQUIRE(atomic_data_.at(70).isotopes.at(176).isonum == 176);
        REQUIRE(atomic_data_.at(70).isotopes.at(176).mass == Approx(175.9425799999999924));
        REQUIRE(atomic_data_.at(70).isotopes.at(176).abundance == Approx(0.1288700000000000));
    }
    SECTION("Atom # 71")
    {
        REQUIRE(atomic_data_.at(71).Z == 71);
        REQUIRE(atomic_data_.at(71).sym == "lu");
        REQUIRE(atomic_data_.at(71).name == "lutetium");
        REQUIRE(atomic_data_.at(71).multiplicity == 2);
        REQUIRE(atomic_data_.at(71).mass == Approx(174.9668000000000063));
        REQUIRE(atomic_data_.at(71).covradius == Approx(3.5337878546061385));
        REQUIRE(atomic_data_.at(71).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(71).isotopes.at(175).isonum == 175);
        REQUIRE(atomic_data_.at(71).isotopes.at(175).mass == Approx(174.9407799999999895));
        REQUIRE(atomic_data_.at(71).isotopes.at(175).abundance == Approx(0.9740100000000000));
        REQUIRE(atomic_data_.at(71).isotopes.at(176).isonum == 176);
        REQUIRE(atomic_data_.at(71).isotopes.at(176).mass == Approx(175.9426899999999989));
        REQUIRE(atomic_data_.at(71).isotopes.at(176).abundance == Approx(0.0259900000000000));
    }
    SECTION("Atom # 72")
    {
        REQUIRE(atomic_data_.at(72).Z == 72);
        REQUIRE(atomic_data_.at(72).sym == "hf");
        REQUIRE(atomic_data_.at(72).name == "hafnium");
        REQUIRE(atomic_data_.at(72).multiplicity == 3);
        REQUIRE(atomic_data_.at(72).mass == Approx(178.4900000000000091));
        REQUIRE(atomic_data_.at(72).covradius == Approx(3.3070207195511991));
        REQUIRE(atomic_data_.at(72).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(72).isotopes.at(174).isonum == 174);
        REQUIRE(atomic_data_.at(72).isotopes.at(174).mass == Approx(173.9400500000000136));
        REQUIRE(atomic_data_.at(72).isotopes.at(174).abundance == Approx(0.0016000000000000));
        REQUIRE(atomic_data_.at(72).isotopes.at(176).isonum == 176);
        REQUIRE(atomic_data_.at(72).isotopes.at(176).mass == Approx(175.9414099999999905));
        REQUIRE(atomic_data_.at(72).isotopes.at(176).abundance == Approx(0.0526000000000000));
        REQUIRE(atomic_data_.at(72).isotopes.at(177).isonum == 177);
        REQUIRE(atomic_data_.at(72).isotopes.at(177).mass == Approx(176.9432299999999998));
        REQUIRE(atomic_data_.at(72).isotopes.at(177).abundance == Approx(0.1860000000000000));
        REQUIRE(atomic_data_.at(72).isotopes.at(178).isonum == 178);
        REQUIRE(atomic_data_.at(72).isotopes.at(178).mass == Approx(177.9437100000000100));
        REQUIRE(atomic_data_.at(72).isotopes.at(178).abundance == Approx(0.2728000000000000));
        REQUIRE(atomic_data_.at(72).isotopes.at(179).isonum == 179);
        REQUIRE(atomic_data_.at(72).isotopes.at(179).mass == Approx(178.9458199999999977));
        REQUIRE(atomic_data_.at(72).isotopes.at(179).abundance == Approx(0.1362000000000000));
        REQUIRE(atomic_data_.at(72).isotopes.at(180).isonum == 180);
        REQUIRE(atomic_data_.at(72).isotopes.at(180).mass == Approx(179.9465600000000052));
        REQUIRE(atomic_data_.at(72).isotopes.at(180).abundance == Approx(0.3508000000000000));
    }
    SECTION("Atom # 73")
    {
        REQUIRE(atomic_data_.at(73).Z == 73);
        REQUIRE(atomic_data_.at(73).sym == "ta");
        REQUIRE(atomic_data_.at(73).name == "tantalum");
        REQUIRE(atomic_data_.at(73).multiplicity == 4);
        REQUIRE(atomic_data_.at(73).mass == Approx(180.9478799999999978));
        REQUIRE(atomic_data_.at(73).covradius == Approx(3.2125344132783078));
        REQUIRE(atomic_data_.at(73).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(73).isotopes.at(180).isonum == 180);
        REQUIRE(atomic_data_.at(73).isotopes.at(180).mass == Approx(179.9474600000000066));
        REQUIRE(atomic_data_.at(73).isotopes.at(180).abundance == Approx(0.0001201000000000));
        REQUIRE(atomic_data_.at(73).isotopes.at(181).isonum == 181);
        REQUIRE(atomic_data_.at(73).isotopes.at(181).mass == Approx(180.9480000000000075));
        REQUIRE(atomic_data_.at(73).isotopes.at(181).abundance == Approx(0.9998799000000000));
    }
    SECTION("Atom # 74")
    {
        REQUIRE(atomic_data_.at(74).Z == 74);
        REQUIRE(atomic_data_.at(74).sym == "w");
        REQUIRE(atomic_data_.at(74).name == "tungsten");
        REQUIRE(atomic_data_.at(74).multiplicity == 5);
        REQUIRE(atomic_data_.at(74).mass == Approx(183.8400000000000034));
        REQUIRE(atomic_data_.at(74).covradius == Approx(3.0613563232416814));
        REQUIRE(atomic_data_.at(74).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(74).isotopes.at(180).isonum == 180);
        REQUIRE(atomic_data_.at(74).isotopes.at(180).mass == Approx(179.9467099999999959));
        REQUIRE(atomic_data_.at(74).isotopes.at(180).abundance == Approx(0.0012000000000000));
        REQUIRE(atomic_data_.at(74).isotopes.at(182).isonum == 182);
        REQUIRE(atomic_data_.at(74).isotopes.at(182).mass == Approx(181.9482040000000040));
        REQUIRE(atomic_data_.at(74).isotopes.at(182).abundance == Approx(0.2650000000000000));
        REQUIRE(atomic_data_.at(74).isotopes.at(183).isonum == 183);
        REQUIRE(atomic_data_.at(74).isotopes.at(183).mass == Approx(182.9502229999999940));
        REQUIRE(atomic_data_.at(74).isotopes.at(183).abundance == Approx(0.1431000000000000));
        REQUIRE(atomic_data_.at(74).isotopes.at(184).isonum == 184);
        REQUIRE(atomic_data_.at(74).isotopes.at(184).mass == Approx(183.9509309999999971));
        REQUIRE(atomic_data_.at(74).isotopes.at(184).abundance == Approx(0.3064000000000000));
        REQUIRE(atomic_data_.at(74).isotopes.at(186).isonum == 186);
        REQUIRE(atomic_data_.at(74).isotopes.at(186).mass == Approx(185.9543600000000083));
        REQUIRE(atomic_data_.at(74).isotopes.at(186).abundance == Approx(0.2843000000000000));
    }
    SECTION("Atom # 75")
    {
        REQUIRE(atomic_data_.at(75).Z == 75);
        REQUIRE(atomic_data_.at(75).sym == "re");
        REQUIRE(atomic_data_.at(75).name == "rhenium");
        REQUIRE(atomic_data_.at(75).multiplicity == 6);
        REQUIRE(atomic_data_.at(75).mass == Approx(186.2069999999999936));
        REQUIRE(atomic_data_.at(75).covradius == Approx(2.8534864494413203));
        REQUIRE(atomic_data_.at(75).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(75).isotopes.at(185).isonum == 185);
        REQUIRE(atomic_data_.at(75).isotopes.at(185).mass == Approx(184.9529550000000029));
        REQUIRE(atomic_data_.at(75).isotopes.at(185).abundance == Approx(0.3740000000000000));
        REQUIRE(atomic_data_.at(75).isotopes.at(187).isonum == 187);
        REQUIRE(atomic_data_.at(75).isotopes.at(187).mass == Approx(186.9557499999999948));
        REQUIRE(atomic_data_.at(75).isotopes.at(187).abundance == Approx(0.6260000000000000));
    }
    SECTION("Atom # 76")
    {
        REQUIRE(atomic_data_.at(76).Z == 76);
        REQUIRE(atomic_data_.at(76).sym == "os");
        REQUIRE(atomic_data_.at(76).name == "osmium");
        REQUIRE(atomic_data_.at(76).multiplicity == 5);
        REQUIRE(atomic_data_.at(76).mass == Approx(190.2299999999999898));
        REQUIRE(atomic_data_.at(76).covradius == Approx(2.7212056206592723));
        REQUIRE(atomic_data_.at(76).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(76).isotopes.at(184).isonum == 184);
        REQUIRE(atomic_data_.at(76).isotopes.at(184).mass == Approx(183.9524890000000141));
        REQUIRE(atomic_data_.at(76).isotopes.at(184).abundance == Approx(0.0002000000000000));
        REQUIRE(atomic_data_.at(76).isotopes.at(186).isonum == 186);
        REQUIRE(atomic_data_.at(76).isotopes.at(186).mass == Approx(185.9538400000000138));
        REQUIRE(atomic_data_.at(76).isotopes.at(186).abundance == Approx(0.0159000000000000));
        REQUIRE(atomic_data_.at(76).isotopes.at(187).isonum == 187);
        REQUIRE(atomic_data_.at(76).isotopes.at(187).mass == Approx(186.9557499999999948));
        REQUIRE(atomic_data_.at(76).isotopes.at(187).abundance == Approx(0.0196000000000000));
        REQUIRE(atomic_data_.at(76).isotopes.at(188).isonum == 188);
        REQUIRE(atomic_data_.at(76).isotopes.at(188).mass == Approx(187.9558399999999949));
        REQUIRE(atomic_data_.at(76).isotopes.at(188).abundance == Approx(0.1324000000000000));
        REQUIRE(atomic_data_.at(76).isotopes.at(189).isonum == 189);
        REQUIRE(atomic_data_.at(76).isotopes.at(189).mass == Approx(188.9581399999999860));
        REQUIRE(atomic_data_.at(76).isotopes.at(189).abundance == Approx(0.1615000000000000));
        REQUIRE(atomic_data_.at(76).isotopes.at(190).isonum == 190);
        REQUIRE(atomic_data_.at(76).isotopes.at(190).mass == Approx(189.9584399999999960));
        REQUIRE(atomic_data_.at(76).isotopes.at(190).abundance == Approx(0.2626000000000000));
        REQUIRE(atomic_data_.at(76).isotopes.at(192).isonum == 192);
        REQUIRE(atomic_data_.at(76).isotopes.at(192).mass == Approx(191.9614799999999946));
        REQUIRE(atomic_data_.at(76).isotopes.at(192).abundance == Approx(0.4078000000000000));
    }
    SECTION("Atom # 77")
    {
        REQUIRE(atomic_data_.at(77).Z == 77);
        REQUIRE(atomic_data_.at(77).sym == "ir");
        REQUIRE(atomic_data_.at(77).name == "iridium");
        REQUIRE(atomic_data_.at(77).multiplicity == 4);
        REQUIRE(atomic_data_.at(77).mass == Approx(192.2170000000000130));
        REQUIRE(atomic_data_.at(77).covradius == Approx(2.6645138368955377));
        REQUIRE(atomic_data_.at(77).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(77).isotopes.at(191).isonum == 191);
        REQUIRE(atomic_data_.at(77).isotopes.at(191).mass == Approx(190.9605899999999963));
        REQUIRE(atomic_data_.at(77).isotopes.at(191).abundance == Approx(0.3730000000000000));
        REQUIRE(atomic_data_.at(77).isotopes.at(193).isonum == 193);
        REQUIRE(atomic_data_.at(77).isotopes.at(193).mass == Approx(192.9629199999999969));
        REQUIRE(atomic_data_.at(77).isotopes.at(193).abundance == Approx(0.6270000000000000));
    }
    SECTION("Atom # 78")
    {
        REQUIRE(atomic_data_.at(78).Z == 78);
        REQUIRE(atomic_data_.at(78).sym == "pt");
        REQUIRE(atomic_data_.at(78).name == "platinum");
        REQUIRE(atomic_data_.at(78).multiplicity == 3);
        REQUIRE(atomic_data_.at(78).mass == Approx(195.0840000000000032));
        REQUIRE(atomic_data_.at(78).covradius == Approx(2.5700275306226463));
        REQUIRE(atomic_data_.at(78).vdwradius == Approx(3.3070207195511991));
        REQUIRE(atomic_data_.at(78).isotopes.at(190).isonum == 190);
        REQUIRE(atomic_data_.at(78).isotopes.at(190).mass == Approx(189.9599300000000142));
        REQUIRE(atomic_data_.at(78).isotopes.at(190).abundance == Approx(0.0001200000000000));
        REQUIRE(atomic_data_.at(78).isotopes.at(192).isonum == 192);
        REQUIRE(atomic_data_.at(78).isotopes.at(192).mass == Approx(191.9610399999999970));
        REQUIRE(atomic_data_.at(78).isotopes.at(192).abundance == Approx(0.0078200000000000));
        REQUIRE(atomic_data_.at(78).isotopes.at(194).isonum == 194);
        REQUIRE(atomic_data_.at(78).isotopes.at(194).mass == Approx(193.9626810000000035));
        REQUIRE(atomic_data_.at(78).isotopes.at(194).abundance == Approx(0.3286400000000000));
        REQUIRE(atomic_data_.at(78).isotopes.at(195).isonum == 195);
        REQUIRE(atomic_data_.at(78).isotopes.at(195).mass == Approx(194.9647919999999885));
        REQUIRE(atomic_data_.at(78).isotopes.at(195).abundance == Approx(0.3377500000000000));
        REQUIRE(atomic_data_.at(78).isotopes.at(196).isonum == 196);
        REQUIRE(atomic_data_.at(78).isotopes.at(196).mass == Approx(195.9649520000000109));
        REQUIRE(atomic_data_.at(78).isotopes.at(196).abundance == Approx(0.2521100000000000));
        REQUIRE(atomic_data_.at(78).isotopes.at(198).isonum == 198);
        REQUIRE(atomic_data_.at(78).isotopes.at(198).mass == Approx(197.9678900000000112));
        REQUIRE(atomic_data_.at(78).isotopes.at(198).abundance == Approx(0.0735600000000000));
    }
    SECTION("Atom # 79")
    {
        REQUIRE(atomic_data_.at(79).Z == 79);
        REQUIRE(atomic_data_.at(79).sym == "au");
        REQUIRE(atomic_data_.at(79).name == "gold");
        REQUIRE(atomic_data_.at(79).multiplicity == 2);
        REQUIRE(atomic_data_.at(79).mass == Approx(196.9665689999999927));
        REQUIRE(atomic_data_.at(79).covradius == Approx(2.5700275306226463));
        REQUIRE(atomic_data_.at(79).vdwradius == Approx(3.1369453682599948));
        REQUIRE(atomic_data_.at(79).isotopes.at(197).isonum == 197);
        REQUIRE(atomic_data_.at(79).isotopes.at(197).mass == Approx(196.9665689999999927));
        REQUIRE(atomic_data_.at(79).isotopes.at(197).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 80")
    {
        REQUIRE(atomic_data_.at(80).Z == 80);
        REQUIRE(atomic_data_.at(80).sym == "hg");
        REQUIRE(atomic_data_.at(80).name == "mercury");
        REQUIRE(atomic_data_.at(80).multiplicity == 1);
        REQUIRE(atomic_data_.at(80).mass == Approx(200.5920000000000130));
        REQUIRE(atomic_data_.at(80).covradius == Approx(2.4944384856043329));
        REQUIRE(atomic_data_.at(80).vdwradius == Approx(2.9290754944596338));
        REQUIRE(atomic_data_.at(80).isotopes.at(196).isonum == 196);
        REQUIRE(atomic_data_.at(80).isotopes.at(196).mass == Approx(195.9658300000000111));
        REQUIRE(atomic_data_.at(80).isotopes.at(196).abundance == Approx(0.0015000000000000));
        REQUIRE(atomic_data_.at(80).isotopes.at(198).isonum == 198);
        REQUIRE(atomic_data_.at(80).isotopes.at(198).mass == Approx(197.9667689999999993));
        REQUIRE(atomic_data_.at(80).isotopes.at(198).abundance == Approx(0.1004000000000000));
        REQUIRE(atomic_data_.at(80).isotopes.at(199).isonum == 199);
        REQUIRE(atomic_data_.at(80).isotopes.at(199).mass == Approx(198.9682809999999904));
        REQUIRE(atomic_data_.at(80).isotopes.at(199).abundance == Approx(0.1694000000000000));
        REQUIRE(atomic_data_.at(80).isotopes.at(200).isonum == 200);
        REQUIRE(atomic_data_.at(80).isotopes.at(200).mass == Approx(199.9683269999999879));
        REQUIRE(atomic_data_.at(80).isotopes.at(200).abundance == Approx(0.2314000000000000));
        REQUIRE(atomic_data_.at(80).isotopes.at(201).isonum == 201);
        REQUIRE(atomic_data_.at(80).isotopes.at(201).mass == Approx(200.9703030000000012));
        REQUIRE(atomic_data_.at(80).isotopes.at(201).abundance == Approx(0.1317000000000000));
        REQUIRE(atomic_data_.at(80).isotopes.at(202).isonum == 202);
        REQUIRE(atomic_data_.at(80).isotopes.at(202).mass == Approx(201.9706429999999955));
        REQUIRE(atomic_data_.at(80).isotopes.at(202).abundance == Approx(0.2974000000000000));
        REQUIRE(atomic_data_.at(80).isotopes.at(204).isonum == 204);
        REQUIRE(atomic_data_.at(80).isotopes.at(204).mass == Approx(203.9734939999999881));
        REQUIRE(atomic_data_.at(80).isotopes.at(204).abundance == Approx(0.0682000000000000));
    }
    SECTION("Atom # 81")
    {
        REQUIRE(atomic_data_.at(81).Z == 81);
        REQUIRE(atomic_data_.at(81).sym == "tl");
        REQUIRE(atomic_data_.at(81).name == "thallium");
        REQUIRE(atomic_data_.at(81).multiplicity == 2);
        REQUIRE(atomic_data_.at(81).mass == Approx(204.3834999999999980));
        REQUIRE(atomic_data_.at(81).covradius == Approx(2.7401028819138507));
        REQUIRE(atomic_data_.at(81).vdwradius == Approx(3.7038632058973433));
        REQUIRE(atomic_data_.at(81).isotopes.at(203).isonum == 203);
        REQUIRE(atomic_data_.at(81).isotopes.at(203).mass == Approx(202.9723449999999900));
        REQUIRE(atomic_data_.at(81).isotopes.at(203).abundance == Approx(0.2951500000000000));
        REQUIRE(atomic_data_.at(81).isotopes.at(205).isonum == 205);
        REQUIRE(atomic_data_.at(81).isotopes.at(205).mass == Approx(204.9744279999999890));
        REQUIRE(atomic_data_.at(81).isotopes.at(205).abundance == Approx(0.7048500000000000));
    }
    SECTION("Atom # 82")
    {
        REQUIRE(atomic_data_.at(82).Z == 82);
        REQUIRE(atomic_data_.at(82).sym == "pb");
        REQUIRE(atomic_data_.at(82).name == "lead");
        REQUIRE(atomic_data_.at(82).multiplicity == 3);
        REQUIRE(atomic_data_.at(82).mass == Approx(207.1999999999999886));
        REQUIRE(atomic_data_.at(82).covradius == Approx(2.7590001431684290));
        REQUIRE(atomic_data_.at(82).vdwradius == Approx(3.8172467734248130));
        REQUIRE(atomic_data_.at(82).isotopes.at(204).isonum == 204);
        REQUIRE(atomic_data_.at(82).isotopes.at(204).mass == Approx(203.9730439999999874));
        REQUIRE(atomic_data_.at(82).isotopes.at(204).abundance == Approx(0.0140000000000000));
        REQUIRE(atomic_data_.at(82).isotopes.at(206).isonum == 206);
        REQUIRE(atomic_data_.at(82).isotopes.at(206).mass == Approx(205.9744660000000067));
        REQUIRE(atomic_data_.at(82).isotopes.at(206).abundance == Approx(0.2410000000000000));
        REQUIRE(atomic_data_.at(82).isotopes.at(207).isonum == 207);
        REQUIRE(atomic_data_.at(82).isotopes.at(207).mass == Approx(206.9758970000000033));
        REQUIRE(atomic_data_.at(82).isotopes.at(207).abundance == Approx(0.2210000000000000));
        REQUIRE(atomic_data_.at(82).isotopes.at(208).isonum == 208);
        REQUIRE(atomic_data_.at(82).isotopes.at(208).mass == Approx(207.9766529999999989));
        REQUIRE(atomic_data_.at(82).isotopes.at(208).abundance == Approx(0.5240000000000000));
    }
    SECTION("Atom # 83")
    {
        REQUIRE(atomic_data_.at(83).Z == 83);
        REQUIRE(atomic_data_.at(83).sym == "bi");
        REQUIRE(atomic_data_.at(83).name == "bismuth");
        REQUIRE(atomic_data_.at(83).multiplicity == 4);
        REQUIRE(atomic_data_.at(83).mass == Approx(208.9804000000000030));
        REQUIRE(atomic_data_.at(83).covradius == Approx(2.7967946656775857));
        REQUIRE(atomic_data_.at(83).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(83).isotopes.at(209).isonum == 209);
        REQUIRE(atomic_data_.at(83).isotopes.at(209).mass == Approx(208.9804000000000030));
        REQUIRE(atomic_data_.at(83).isotopes.at(209).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 84")
    {
        REQUIRE(atomic_data_.at(84).Z == 84);
        REQUIRE(atomic_data_.at(84).sym == "po");
        REQUIRE(atomic_data_.at(84).name == "polonium");
        REQUIRE(atomic_data_.at(84).multiplicity == 3);
        REQUIRE(atomic_data_.at(84).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(84).covradius == Approx(2.6456165756409593));
        REQUIRE(atomic_data_.at(84).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 85")
    {
        REQUIRE(atomic_data_.at(85).Z == 85);
        REQUIRE(atomic_data_.at(85).sym == "at");
        REQUIRE(atomic_data_.at(85).name == "astatine");
        REQUIRE(atomic_data_.at(85).multiplicity == 2);
        REQUIRE(atomic_data_.at(85).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(85).covradius == Approx(2.8345891881867420));
        REQUIRE(atomic_data_.at(85).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 86")
    {
        REQUIRE(atomic_data_.at(86).Z == 86);
        REQUIRE(atomic_data_.at(86).sym == "rn");
        REQUIRE(atomic_data_.at(86).name == "radon");
        REQUIRE(atomic_data_.at(86).multiplicity == 1);
        REQUIRE(atomic_data_.at(86).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(86).covradius == Approx(2.8345891881867420));
        REQUIRE(atomic_data_.at(86).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 87")
    {
        REQUIRE(atomic_data_.at(87).Z == 87);
        REQUIRE(atomic_data_.at(87).sym == "fr");
        REQUIRE(atomic_data_.at(87).name == "francium");
        REQUIRE(atomic_data_.at(87).multiplicity == 2);
        REQUIRE(atomic_data_.at(87).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(87).covradius == Approx(4.9132879261903533));
        REQUIRE(atomic_data_.at(87).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 88")
    {
        REQUIRE(atomic_data_.at(88).Z == 88);
        REQUIRE(atomic_data_.at(88).sym == "ra");
        REQUIRE(atomic_data_.at(88).name == "radium");
        REQUIRE(atomic_data_.at(88).multiplicity == 1);
        REQUIRE(atomic_data_.at(88).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(88).covradius == Approx(4.1762947372618004));
        REQUIRE(atomic_data_.at(88).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 89")
    {
        REQUIRE(atomic_data_.at(89).Z == 89);
        REQUIRE(atomic_data_.at(89).sym == "ac");
        REQUIRE(atomic_data_.at(89).name == "actinium");
        REQUIRE(atomic_data_.at(89).multiplicity == 2);
        REQUIRE(atomic_data_.at(89).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(89).covradius == Approx(4.0629111697343303));
        REQUIRE(atomic_data_.at(89).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 90")
    {
        REQUIRE(atomic_data_.at(90).Z == 90);
        REQUIRE(atomic_data_.at(90).sym == "th");
        REQUIRE(atomic_data_.at(90).name == "thorium");
        REQUIRE(atomic_data_.at(90).multiplicity == 3);
        REQUIRE(atomic_data_.at(90).mass == Approx(232.0377000000000010));
        REQUIRE(atomic_data_.at(90).covradius == Approx(3.8928358184431260));
        REQUIRE(atomic_data_.at(90).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(90).isotopes.at(230).isonum == 230);
        REQUIRE(atomic_data_.at(90).isotopes.at(230).mass == Approx(230.0331299999999999));
        REQUIRE(atomic_data_.at(90).isotopes.at(230).abundance == Approx(0.0002000000000000));
        REQUIRE(atomic_data_.at(90).isotopes.at(232).isonum == 232);
        REQUIRE(atomic_data_.at(90).isotopes.at(232).mass == Approx(232.0380600000000015));
        REQUIRE(atomic_data_.at(90).isotopes.at(232).abundance == Approx(0.9998000000000000));
    }
    SECTION("Atom # 91")
    {
        REQUIRE(atomic_data_.at(91).Z == 91);
        REQUIRE(atomic_data_.at(91).sym == "pa");
        REQUIRE(atomic_data_.at(91).name == "protactinium");
        REQUIRE(atomic_data_.at(91).multiplicity == 4);
        REQUIRE(atomic_data_.at(91).mass == Approx(231.0358799999999917));
        REQUIRE(atomic_data_.at(91).covradius == Approx(3.7794522509156563));
        REQUIRE(atomic_data_.at(91).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(91).isotopes.at(231).isonum == 231);
        REQUIRE(atomic_data_.at(91).isotopes.at(231).mass == Approx(231.0358799999999917));
        REQUIRE(atomic_data_.at(91).isotopes.at(231).abundance == Approx(1.0000000000000000));
    }
    SECTION("Atom # 92")
    {
        REQUIRE(atomic_data_.at(92).Z == 92);
        REQUIRE(atomic_data_.at(92).sym == "u");
        REQUIRE(atomic_data_.at(92).name == "uranium");
        REQUIRE(atomic_data_.at(92).multiplicity == 5);
        REQUIRE(atomic_data_.at(92).mass == Approx(238.0289099999999962));
        REQUIRE(atomic_data_.at(92).covradius == Approx(3.7038632058973433));
        REQUIRE(atomic_data_.at(92).vdwradius == Approx(3.5148905933515602));
        REQUIRE(atomic_data_.at(92).isotopes.at(233).isonum == 233);
        REQUIRE(atomic_data_.at(92).isotopes.at(233).mass == Approx(233.0396399999999915));
        REQUIRE(atomic_data_.at(92).isotopes.at(233).abundance == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(92).isotopes.at(234).isonum == 234);
        REQUIRE(atomic_data_.at(92).isotopes.at(234).mass == Approx(234.0409500000000094));
        REQUIRE(atomic_data_.at(92).isotopes.at(234).abundance == Approx(0.0000540000000000));
        REQUIRE(atomic_data_.at(92).isotopes.at(235).isonum == 235);
        REQUIRE(atomic_data_.at(92).isotopes.at(235).mass == Approx(235.0439299999999889));
        REQUIRE(atomic_data_.at(92).isotopes.at(235).abundance == Approx(0.0072040000000000));
        REQUIRE(atomic_data_.at(92).isotopes.at(238).isonum == 238);
        REQUIRE(atomic_data_.at(92).isotopes.at(238).mass == Approx(238.0507900000000063));
        REQUIRE(atomic_data_.at(92).isotopes.at(238).abundance == Approx(0.9927420000000000));
    }
    SECTION("Atom # 93")
    {
        REQUIRE(atomic_data_.at(93).Z == 93);
        REQUIRE(atomic_data_.at(93).sym == "np");
        REQUIRE(atomic_data_.at(93).name == "neptunium");
        REQUIRE(atomic_data_.at(93).multiplicity == 6);
        REQUIRE(atomic_data_.at(93).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(93).covradius == Approx(3.5904796383698736));
        REQUIRE(atomic_data_.at(93).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 94")
    {
        REQUIRE(atomic_data_.at(94).Z == 94);
        REQUIRE(atomic_data_.at(94).sym == "pu");
        REQUIRE(atomic_data_.at(94).name == "plutonium");
        REQUIRE(atomic_data_.at(94).multiplicity == 7);
        REQUIRE(atomic_data_.at(94).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(94).covradius == Approx(3.5337878546061385));
        REQUIRE(atomic_data_.at(94).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 95")
    {
        REQUIRE(atomic_data_.at(95).Z == 95);
        REQUIRE(atomic_data_.at(95).sym == "am");
        REQUIRE(atomic_data_.at(95).name == "americium");
        REQUIRE(atomic_data_.at(95).multiplicity == 8);
        REQUIRE(atomic_data_.at(95).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(95).covradius == Approx(3.4015070258240905));
        REQUIRE(atomic_data_.at(95).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 96")
    {
        REQUIRE(atomic_data_.at(96).Z == 96);
        REQUIRE(atomic_data_.at(96).sym == "cm");
        REQUIRE(atomic_data_.at(96).name == "curium");
        REQUIRE(atomic_data_.at(96).multiplicity == 9);
        REQUIRE(atomic_data_.at(96).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(96).covradius == Approx(3.1936371520237294));
        REQUIRE(atomic_data_.at(96).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 97")
    {
        REQUIRE(atomic_data_.at(97).Z == 97);
        REQUIRE(atomic_data_.at(97).sym == "bk");
        REQUIRE(atomic_data_.at(97).name == "berkelium");
        REQUIRE(atomic_data_.at(97).multiplicity == 6);
        REQUIRE(atomic_data_.at(97).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(97).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(97).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 98")
    {
        REQUIRE(atomic_data_.at(98).Z == 98);
        REQUIRE(atomic_data_.at(98).sym == "cf");
        REQUIRE(atomic_data_.at(98).name == "californium");
        REQUIRE(atomic_data_.at(98).multiplicity == 5);
        REQUIRE(atomic_data_.at(98).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(98).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(98).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 99")
    {
        REQUIRE(atomic_data_.at(99).Z == 99);
        REQUIRE(atomic_data_.at(99).sym == "es");
        REQUIRE(atomic_data_.at(99).name == "einsteinium");
        REQUIRE(atomic_data_.at(99).multiplicity == 4);
        REQUIRE(atomic_data_.at(99).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(99).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(99).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 100")
    {
        REQUIRE(atomic_data_.at(100).Z == 100);
        REQUIRE(atomic_data_.at(100).sym == "fm");
        REQUIRE(atomic_data_.at(100).name == "fermium");
        REQUIRE(atomic_data_.at(100).multiplicity == 3);
        REQUIRE(atomic_data_.at(100).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(100).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(100).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 101")
    {
        REQUIRE(atomic_data_.at(101).Z == 101);
        REQUIRE(atomic_data_.at(101).sym == "md");
        REQUIRE(atomic_data_.at(101).name == "mendelevium");
        REQUIRE(atomic_data_.at(101).multiplicity == 2);
        REQUIRE(atomic_data_.at(101).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(101).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(101).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 102")
    {
        REQUIRE(atomic_data_.at(102).Z == 102);
        REQUIRE(atomic_data_.at(102).sym == "no");
        REQUIRE(atomic_data_.at(102).name == "nobelium");
        REQUIRE(atomic_data_.at(102).multiplicity == 1);
        REQUIRE(atomic_data_.at(102).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(102).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(102).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 103")
    {
        REQUIRE(atomic_data_.at(103).Z == 103);
        REQUIRE(atomic_data_.at(103).sym == "lr");
        REQUIRE(atomic_data_.at(103).name == "lawrencium");
        REQUIRE(atomic_data_.at(103).multiplicity == 2);
        REQUIRE(atomic_data_.at(103).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(103).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(103).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 104")
    {
        REQUIRE(atomic_data_.at(104).Z == 104);
        REQUIRE(atomic_data_.at(104).sym == "rf");
        REQUIRE(atomic_data_.at(104).name == "rutherfordium");
        REQUIRE(atomic_data_.at(104).multiplicity == 3);
        REQUIRE(atomic_data_.at(104).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(104).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(104).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 105")
    {
        REQUIRE(atomic_data_.at(105).Z == 105);
        REQUIRE(atomic_data_.at(105).sym == "db");
        REQUIRE(atomic_data_.at(105).name == "dubnium");
        REQUIRE(atomic_data_.at(105).multiplicity == 4);
        REQUIRE(atomic_data_.at(105).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(105).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(105).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 106")
    {
        REQUIRE(atomic_data_.at(106).Z == 106);
        REQUIRE(atomic_data_.at(106).sym == "sg");
        REQUIRE(atomic_data_.at(106).name == "seaborgium");
        REQUIRE(atomic_data_.at(106).multiplicity == 0);
        REQUIRE(atomic_data_.at(106).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(106).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(106).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 107")
    {
        REQUIRE(atomic_data_.at(107).Z == 107);
        REQUIRE(atomic_data_.at(107).sym == "bh");
        REQUIRE(atomic_data_.at(107).name == "bohrium");
        REQUIRE(atomic_data_.at(107).multiplicity == 0);
        REQUIRE(atomic_data_.at(107).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(107).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(107).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 108")
    {
        REQUIRE(atomic_data_.at(108).Z == 108);
        REQUIRE(atomic_data_.at(108).sym == "hs");
        REQUIRE(atomic_data_.at(108).name == "hassium");
        REQUIRE(atomic_data_.at(108).multiplicity == 0);
        REQUIRE(atomic_data_.at(108).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(108).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(108).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 109")
    {
        REQUIRE(atomic_data_.at(109).Z == 109);
        REQUIRE(atomic_data_.at(109).sym == "mt");
        REQUIRE(atomic_data_.at(109).name == "meitnerium");
        REQUIRE(atomic_data_.at(109).multiplicity == 0);
        REQUIRE(atomic_data_.at(109).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(109).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(109).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 110")
    {
        REQUIRE(atomic_data_.at(110).Z == 110);
        REQUIRE(atomic_data_.at(110).sym == "ds");
        REQUIRE(atomic_data_.at(110).name == "darmstadtium");
        REQUIRE(atomic_data_.at(110).multiplicity == 0);
        REQUIRE(atomic_data_.at(110).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(110).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(110).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 111")
    {
        REQUIRE(atomic_data_.at(111).Z == 111);
        REQUIRE(atomic_data_.at(111).sym == "rg");
        REQUIRE(atomic_data_.at(111).name == "roentgenium");
        REQUIRE(atomic_data_.at(111).multiplicity == 0);
        REQUIRE(atomic_data_.at(111).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(111).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(111).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 112")
    {
        REQUIRE(atomic_data_.at(112).Z == 112);
        REQUIRE(atomic_data_.at(112).sym == "cn");
        REQUIRE(atomic_data_.at(112).name == "copernicium");
        REQUIRE(atomic_data_.at(112).multiplicity == 0);
        REQUIRE(atomic_data_.at(112).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(112).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(112).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 113")
    {
        REQUIRE(atomic_data_.at(113).Z == 113);
        REQUIRE(atomic_data_.at(113).sym == "nh");
        REQUIRE(atomic_data_.at(113).name == "nihonium");
        REQUIRE(atomic_data_.at(113).multiplicity == 0);
        REQUIRE(atomic_data_.at(113).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(113).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(113).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 114")
    {
        REQUIRE(atomic_data_.at(114).Z == 114);
        REQUIRE(atomic_data_.at(114).sym == "fl");
        REQUIRE(atomic_data_.at(114).name == "flerovium");
        REQUIRE(atomic_data_.at(114).multiplicity == 0);
        REQUIRE(atomic_data_.at(114).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(114).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(114).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 115")
    {
        REQUIRE(atomic_data_.at(115).Z == 115);
        REQUIRE(atomic_data_.at(115).sym == "mc");
        REQUIRE(atomic_data_.at(115).name == "moscovium");
        REQUIRE(atomic_data_.at(115).multiplicity == 0);
        REQUIRE(atomic_data_.at(115).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(115).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(115).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 116")
    {
        REQUIRE(atomic_data_.at(116).Z == 116);
        REQUIRE(atomic_data_.at(116).sym == "lv");
        REQUIRE(atomic_data_.at(116).name == "livermorium");
        REQUIRE(atomic_data_.at(116).multiplicity == 0);
        REQUIRE(atomic_data_.at(116).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(116).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(116).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 117")
    {
        REQUIRE(atomic_data_.at(117).Z == 117);
        REQUIRE(atomic_data_.at(117).sym == "ts");
        REQUIRE(atomic_data_.at(117).name == "tennessine");
        REQUIRE(atomic_data_.at(117).multiplicity == 0);
        REQUIRE(atomic_data_.at(117).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(117).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(117).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 118")
    {
        REQUIRE(atomic_data_.at(118).Z == 118);
        REQUIRE(atomic_data_.at(118).sym == "og");
        REQUIRE(atomic_data_.at(118).name == "oganesson");
        REQUIRE(atomic_data_.at(118).multiplicity == 0);
        REQUIRE(atomic_data_.at(118).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(118).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(118).vdwradius == Approx(0.0000000000000000));
    }
    SECTION("Atom # 999")
    {
        REQUIRE(atomic_data_.at(999).Z == 999);
        REQUIRE(atomic_data_.at(999).sym == "chg");
        REQUIRE(atomic_data_.at(999).name == "charge");
        REQUIRE(atomic_data_.at(999).multiplicity == 0);
        REQUIRE(atomic_data_.at(999).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(999).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(999).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(999).isotopes.at(999).isonum == 999);
        REQUIRE(atomic_data_.at(999).isotopes.at(999).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(999).isotopes.at(999).abundance == Approx(0.0000000000000000));
    }
    SECTION("Atom # 9999")
    {
        REQUIRE(atomic_data_.at(9999).Z == 9999);
        REQUIRE(atomic_data_.at(9999).sym == "dum");
        REQUIRE(atomic_data_.at(9999).name == "dummy");
        REQUIRE(atomic_data_.at(9999).multiplicity == 0);
        REQUIRE(atomic_data_.at(9999).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(9999).covradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(9999).vdwradius == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(9999).isotopes.at(9999).isonum == 9999);
        REQUIRE(atomic_data_.at(9999).isotopes.at(9999).mass == Approx(0.0000000000000000));
        REQUIRE(atomic_data_.at(9999).isotopes.at(9999).abundance == Approx(0.0000000000000000));
    }
}
