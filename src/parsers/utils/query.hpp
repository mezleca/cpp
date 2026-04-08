#pragma once

#include <bitset>
#include <string_view>

enum class QueryOp : int { INVALID = -1, EQ, NEQ, GT, LT, GTE, LTE };

enum class ParseState : int { KEY, VALUE };

static constexpr std::bitset<256> make_op_start_table() {
    std::bitset<256> table{};

    table.set('>');
    table.set('<');
    table.set('=');
    table.set('!');

    return table;
}

static auto OP_START_TABLE = make_op_start_table();

struct QueryToken {
    std::string_view key;
    std::string_view value;
    QueryOp op;
};

struct QueryState {
    size_t key_start;
    size_t op_start;
    size_t op_end;
    size_t value_end;

    ParseState value = ParseState::KEY;
    QueryOp op = QueryOp::INVALID;
    QueryToken token = {};

    bool hit;

    void reset() {
        value = ParseState::KEY;

        key_start = 0;
        op_start = 0;
        op_end = 0;
        value_end = 0;
    }
};

namespace query {
    inline std::pair<QueryOp, size_t> parse_operator(std::string_view sv, size_t pos) {
        char c1 = sv[pos];
        char c2 = sv[pos + 1];

        switch (c1) {
            case '>': {
                if (c2 == '=') {
                    return {QueryOp::GTE, 2};
                }

                return {QueryOp::GT, 1};
            }

            case '<': {
                if (c2 == '=') {
                    return {QueryOp::LTE, 2};
                }

                return {QueryOp::LT, 1};
            }

            case '=': {
                if (c2 == '=') { // variation
                    return {QueryOp::EQ, 2};
                }

                return {QueryOp::EQ, 1};
            }

            case '!': {
                if (c2 == '=') {
                    return {QueryOp::NEQ, 2};
                }

                return {QueryOp::INVALID, 1};
            }
        }

        return {QueryOp::INVALID, 0};
    }

    template <typename F, typename S> inline std::pair<std::string, bool> parse(std::string_view data, F d, S s) {
        QueryState m_state = {};
        std::string content;

        for (size_t i = 0; i < data.length(); i++) {
            if (m_state.hit) {
                break;
            }

            bool is_last = data.length() - 1 == i;
            char c = data[i];

            switch (m_state.value) {
                case ParseState::KEY: {
                    if (c == ' ' && !is_last) {
                        m_state.key_start = i + 1;
                    }

                    // is a operator?
                    if (m_state.key_start < i && OP_START_TABLE[static_cast<unsigned char>(c)]) {
                        auto [op, size] = parse_operator(data, i);
                        m_state.op = op;

                        // invalidate duplicated op's
                        if (!is_last && c != '=' && data[i + 1] == c) {
                            content += content.substr(m_state.key_start, i - 1);
                        } else {
                            m_state.value = ParseState::VALUE;
                            m_state.op_start = i;
                            m_state.op_end = m_state.op_start + size;
                        }

                        continue;
                    }

                    if (m_state.key_start == 0) {
                        m_state.key_start = i;
                    }
                } break;
                case ParseState::VALUE: {
                    if (c == ' ' || is_last) {
                        m_state.token = {
                            .key = content.substr(m_state.key_start, m_state.op_start - m_state.key_start),
                            .value = content.substr(m_state.op_end, m_state.value_end - m_state.op_end + 1),
                            .op = m_state.op,
                        };

                        auto eval = d(m_state.token.key, m_state.token.op);

                        if (eval == nullptr) {
                            m_state.reset();
                            continue;
                        }

                        // early exit if the eval hits false
                        if (!eval(s, m_state.token.value)) {
                            m_state.hit = true;
                            break;
                        }

                        m_state.reset();
                        break;
                    }

                    m_state.value_end = i;
                } break;
                default:
                    break;
            }
        }
    }
}