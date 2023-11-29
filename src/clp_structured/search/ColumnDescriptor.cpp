#include "ColumnDescriptor.hpp"

#include <memory>

namespace clp_structured { namespace search {
    DescriptorList tokenize_descriptor(std::vector<std::string> const& descriptors) {
        DescriptorList list;
        for (std::string const& descriptor : descriptors) {
            list.push_back(DescriptorToken(descriptor));
        }
        return list;
    }

    void ColumnDescriptor::check_and_set_unresolved_descriptor_flag() {
        m_unresolved_descriptors = false;
        m_pure_wildcard = m_descriptors.size() == 1 && m_descriptors[0].wildcard();
        for (auto const& token : m_descriptors) {
            if (token.wildcard() || token.regex()) {
                m_unresolved_descriptors = true;
                break;
            }
        }
    }

    ColumnDescriptor::ColumnDescriptor(std::string const& descriptor) {
        m_flags = cAllTypes;
        m_descriptors.emplace_back(descriptor);
        check_and_set_unresolved_descriptor_flag();
    }

    ColumnDescriptor::ColumnDescriptor(std::vector<std::string> const& descriptors) {
        m_flags = cAllTypes;
        m_descriptors = std::move(tokenize_descriptor(descriptors));
        check_and_set_unresolved_descriptor_flag();
    }

    ColumnDescriptor::ColumnDescriptor(DescriptorList const& descriptors) {
        m_flags = cAllTypes;
        m_descriptors = descriptors;
        check_and_set_unresolved_descriptor_flag();
    }

    std::shared_ptr<ColumnDescriptor> ColumnDescriptor::create(std::string const& descriptor) {
        return std::shared_ptr<ColumnDescriptor>(new ColumnDescriptor(descriptor));
    }

    std::shared_ptr<ColumnDescriptor> ColumnDescriptor::create(
            std::vector<std::string> const& descriptors
    ) {
        return std::shared_ptr<ColumnDescriptor>(new ColumnDescriptor(descriptors));
    }

    std::shared_ptr<ColumnDescriptor> ColumnDescriptor::create(DescriptorList const& descriptors) {
        return std::shared_ptr<ColumnDescriptor>(new ColumnDescriptor(descriptors));
    }

    std::shared_ptr<ColumnDescriptor> ColumnDescriptor::copy() {
        return std::make_shared<ColumnDescriptor>(*this);
    }

    void ColumnDescriptor::print() {
        auto& os = get_print_stream();
        os << "ColumnDescriptor<";
        for (uint32_t flag = LiteralType::TypesBegin; flag < LiteralType::TypesEnd; flag <<= 1) {
            if (m_flags & flag) {
                os << Literal::type_to_string(static_cast<LiteralType>(flag));

                // If there are any types remaining add a comma
                if (flag << 1 <= m_flags) {
                    os << ",";
                }
            }
        }
        os << ">(";

        for (auto it = m_descriptors.begin(); it != m_descriptors.end();) {
            os << "\"" << (*it).get_token() << "\"";

            it++;
            if (it != m_descriptors.end()) {
                os << ", ";
            }
        }
        os << ")";
    }

    void ColumnDescriptor::add_unresolved_tokens(DescriptorList::iterator it) {
        m_unresolved_tokens.assign(it, descriptor_end());
    }
}}  // namespace clp_structured::search
