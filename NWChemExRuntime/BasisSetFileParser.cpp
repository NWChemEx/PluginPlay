#include "NWChemExRuntime/BasisSetFileParser.hpp"
#include "NWChemExRuntime/AtomicInfo.hpp"
#include <LibChemist/ShellTypes.hpp>
#include <regex>

namespace NWXRuntime {

    using data_type   = BasisSetFileParser::data_type;
    using action_type = BasisSetFileParser::action_type;
    using return_type = std::map<std::size_t,
        std::vector<LibChemist::BasisShell>>;
    using parsed_type = std::map<data_type, std::vector<double>>;

    namespace detail_ {

// Struct to hold data as we find it
        struct shell {
            // TODO: get correct angular moemntum
            LibChemist::ShellType type =
                LibChemist::ShellType::SphericalGaussian;
            int l;
            size_t ngen;
            std::vector<double> alphas;
            std::vector<std::vector<double>> cs;
        };

// factors out reading of the instance returned by a parser's parse function
        void parse(const parsed_type& data, shell& s)
        {
            if(data.count(data_type::exponent))
                s.alphas.push_back(data.at(data_type::exponent)[0]);
            if(data.count(data_type::coefficient))
            {
                size_t i = 0;
                for(auto x : data.at(data_type::coefficient))
                {
                    if(s.cs.size() <= i)
                        s.cs.push_back(std::vector<double>());
                    s.cs[i++].push_back(x);
                }
            }
            if(data.count(data_type::angular_momentum))
            {
                s.l    = data.at(data_type::angular_momentum)[0];
                s.ngen = (s.l < 0 ? -1 * s.l + 1 : 1);
            }
        }

// Code factorization for adding the shell to our return value
        void commit_shell(shell& s, size_t Z, return_type& rv)
        {
            if(s.alphas.size() != 0 && Z != 0)
            {
                // For general contractions put the coefficients into single vector
                std::vector<double> coefs;
                for(auto x : s.cs)
                    coefs.insert(coefs.end(), x.begin(), x.end());
                //
                rv[Z].push_back(LibChemist::BasisShell(s.type, s.l, s.ngen,
                                                       s.alphas, coefs));
            }
            s = shell();
        }

    } // End namespace detail_

    return_type parse_basis_set_file(std::istream& is,
                                     const BasisSetFileParser& parser)
    {
        return_type rv;
        size_t Z = 0;
        detail_::shell s;
        while(is)
        {
            std::string line;
            std::getline(is, line);
            switch(parser.worth_parsing(line))
            {
                case(action_type::none): // Junk line
                {
                    break;
                }
                case(action_type::new_atom): // Implies new shell
                {
                    detail_::commit_shell(s, Z, rv);
                    auto data = parser.parse(line);
                    Z         = data.at(data_type::Z)[0];
                    detail_::parse(data, s);
                    break;
                }
                case(action_type::new_shell):
                {
                    detail_::commit_shell(s, Z, rv);
                    // Intentional fall-through
                }
                default:
                {
                    auto data = parser.parse(line);
                    detail_::parse(data, s);
                    break;
                }
            } // End switch
        }     // End while
        detail_::commit_shell(s, Z, rv);
        return rv;
    }

    static const std::regex G94_new_atom("^\\s*\\D{1,2}\\s*0\\s*$");
    static const std::regex G94_new_shell("^\\s*[a-zA-Z]+\\s*\\d+\\s*1.00\\s*$");
    static const std::regex G94_same_shell("^\\s*(?:-?\\d+.\\d+\\s*)+$");

    action_type G94::worth_parsing(const std::string& line) const
    {
        if(std::regex_search(line, G94_new_atom))
            return action_type::new_atom;
        else if(std::regex_search(line, G94_new_shell))
            return action_type::new_shell;
        else if(std::regex_search(line, G94_same_shell))
            return action_type::same_shell;
        return action_type::none;
    }

    parsed_type G94::parse(const std::string& line) const
    {
        parsed_type rv;
        std::stringstream tokenizer(line);
        if(std::regex_search(line, G94_new_atom))
        {
            std::string symbol;
            tokenizer >> symbol;
            rv[data_type::Z].push_back(detail_::sym2Z_.at(symbol));
        }
        else if(std::regex_search(line, G94_new_shell))
        {
            std::string am;
            tokenizer >> am;
            std::transform(am.begin(), am.end(), am.begin(), ::tolower);
            rv[data_type::angular_momentum].push_back(LibChemist::am_str2int
            (am));
        }
        else if(std::regex_search(line, G94_same_shell))
        {
            double a, c;
            tokenizer >> a;
            rv[data_type::exponent].push_back(a);
            while(tokenizer >> c)
            {
                rv[data_type::coefficient].push_back(c);
            }
        }
        return rv;
    }

} // namespace LibChemist
