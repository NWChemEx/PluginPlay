// #pragma once

// namespace pluginplay::database::detail_ {

// class TopLevel : public DatabasePIMPL<input_map, result_map> {
// public:
//     bool count(const input_map& inputs) {
//         // Make sure we've seen all these inputs
//         auto input_proxy = m_i2p_->at(inputs);
//         if(!input_proxy) return false;
//         return m_p2r_->count(input_proxy);
//     }

// private:
//     input_mapper_pointer m_i2p_;
//     quick_hit_pointer m_p2r_;

//     result_mapper_pointer m_r2p_;
//     proxy_mapper_pointer m_p2p_;
// }

// } // namespace pluginplay::database::detail_
