class RemoveContFromCount < ActiveRecord::Migration
  def change
    remove_column :counts, :count, :integer
  end
end
