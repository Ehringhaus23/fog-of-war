#include "pseudo_legal_move_generator.h"

namespace chess {

    PseudoLegalMoveGenerator::PseudoLegalMoveGenerator(const Position& position) : m_position(position) {
        m_index = 0;
    }

    PseudoLegalMoveGenerator PseudoLegalMoveGenerator::__iter__() {
        return *this;
    }

    void PseudoLegalMoveGenerator::generate_from_square(Square square) {
        // Skip empty square and opposing pieces.
        Piece piece = m_position.get(square);
        if (!piece.is_valid() || piece.color() != m_position.turn()) {
            return;
        }

        // Pawn moves.
        if (piece.type() == 'p') {
            // Single steps forward.
            Square target = Square::from_x88_index(
                square.x88_index() + ((m_position.turn() == 'b') ? 16 : -16));

            if (!m_position.get(target).is_valid()) {
                 if (target.is_backrank()) {
                    // Promotion.
                    m_cache.push(Move(square, target, 'b'));
                    m_cache.push(Move(square, target, 'n'));
                    m_cache.push(Move(square, target, 'r'));
                    m_cache.push(Move(square, target, 'q'));
                } else {
                    m_cache.push(Move(square, target));

                    // Two steps forward.
                    if ((m_position.turn() == 'w' && square.rank() == 1) ||
                        (m_position.turn() == 'b' && square.rank() == 6))
                    {
                        target = Square::from_x88_index(
                            square.x88_index() + ((m_position.turn() == 'b') ? 32 : -32));
                        if (!m_position.get(target).is_valid()) {
                            m_cache.push(Move(square, target));
                         }
                    }
                }
            }

            // TODO: Pawn captures.

            // TODO: En-passant.

        }
    }

    Move PseudoLegalMoveGenerator::next() {
        while (m_index < 64 && m_cache.empty()) {
            generate_from_square(Square(m_index++));
        }

        if (m_cache.empty()) {
            PyErr_SetNone(PyExc_StopIteration);
            throw boost::python::error_already_set();
        } else {
            Move move = m_cache.front();
            m_cache.pop();
            return move;
        }
    }

}
